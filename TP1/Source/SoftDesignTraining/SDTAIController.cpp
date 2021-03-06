/*
* STDAIController.cpp
* Authors:
	- Sébastien Chagnon
	- Andy Lam
	- Jason Thai
	- Alexandre Ramtoula
	- Philippe Trempe
*/
#include "SDTAIController.h"
#include "SDTCollectible.h"
#include "SoftDesignTrainingMainCharacter.h"
#include "SoftDesignTraining.h"
#include "DrawDebugHelpers.h"
#include "SDTUtils.h"

void ASDTAIController::Tick(float deltaTime)
{
	UWorld* world = GetWorld();

	// Compute agents's feet position
	UMeshComponent* const characterMesh = (UMeshComponent*) GetPawn()->GetComponentByClass(UMeshComponent::StaticClass());
	FVector const feetOffset = characterMesh->GetRelativeLocation();
	FVector const feetCenter = GetPawn()->GetActorLocation() + feetOffset + FVector(0, 0, 2);

	// Compute key positions for raytracing aroud the agent's position
	UCapsuleComponent* const boundingBox = (UCapsuleComponent*)GetPawn()->GetComponentByClass(UCapsuleComponent::StaticClass());
	FVector const rightFoot = feetCenter + (GetPawn()->GetActorRightVector().GetSafeNormal() * boundingBox->GetScaledCapsuleRadius());
	FVector const leftFoot = feetCenter + (GetPawn()->GetActorRightVector().GetSafeNormal() * boundingBox->GetScaledCapsuleRadius() * -1);

	FVector const sight = (GetPawn()->GetActorRotation().Vector().GetSafeNormal() * sightThreshold);
	FVector const rightFootSight = rightFoot + sight / 2;
	FVector const centerSight = feetCenter + sight;
	FVector const leftFootSight = leftFoot + sight / 2;

	if (debug) {
		DrawDebugLine(world, rightFoot, rightFootSight, FColor::Red);
		DrawDebugLine(world, feetCenter, centerSight, FColor::Red);
		DrawDebugLine(world, leftFoot, leftFootSight, FColor::Red);
	}

	// Raytrace from each of the agent's point of view
	FCollisionQueryParams params = FCollisionQueryParams();
	params.AddIgnoredActor(GetPawn());
	
	TArray<struct FHitResult> rightHitResults;
	world->LineTraceMultiByObjectType(rightHitResults, rightFoot, rightFootSight, FCollisionObjectQueryParams::AllObjects, params);
	TArray<struct FHitResult> centerHitResults;
	world->LineTraceMultiByObjectType(centerHitResults, feetCenter, centerSight, FCollisionObjectQueryParams::AllObjects, params);
	TArray<struct FHitResult> leftHitResults;
	world->LineTraceMultiByObjectType(leftHitResults, leftFoot, leftFootSight, FCollisionObjectQueryParams::AllObjects, params);

	bool const hasWallInSight = rightHitResults.Num() + centerHitResults.Num() + leftHitResults.Num() > 0;

	bool collectibleFound = false;
	FVector collectibleLocation;
	bool playerFound = false;
	bool isPlayerPowerUp = false;
	FVector playerLocation;

	// Sweep for collectible or player objects
	sweepForwardObjects(collectibleFound, collectibleLocation, playerFound, isPlayerPowerUp, playerLocation);

	float speed = 1.0f;
	FRotator walkingDirection = GetPawn()->GetActorRotation();

	
	// ***
	// CORNER DETECTION
	// ***
	if (escaping == Escaping::None) {
		// Agent is not currently escaping from a corner

		if (playerFound && isPlayerPowerUp)
		{
			// Agent is escaping from player
			FVector const objectVector = playerLocation - GetPawn()->GetActorLocation();
			FVector crossProduct = FVector::CrossProduct(GetPawn()->GetActorForwardVector(), objectVector);
			escaping = crossProduct.Z > 0 ? Escaping::Left : Escaping::Right;
		}
		else if (rightHitResults.Num() > 0 && leftHitResults.Num() > 0)
		{
			// Left wall collision detection
			float leftWallDistance;
			FVector_NetQuantizeNormal leftWallHitNormal;
			computeNearestCollision(leftWallDistance, leftWallHitNormal, leftHitResults);

			// Right wall collision detection
			float rightWallDistance;
			FVector_NetQuantizeNormal rightWallHitNormal;
			computeNearestCollision(rightWallDistance, rightWallHitNormal, rightHitResults);

			if (!(leftWallHitNormal - rightWallHitNormal).IsNearlyZero()) {
				// Both sides of the agent collision detection are not hitting the same surface normal. Therefore, the agent must be in front of a corner.
				escaping = (leftWallDistance > rightWallDistance) ? Escaping::Left : Escaping::Right;
			}
		}
	}
	else {
		// Agent is currently escaping from a corner

		if (debug) {
			GEngine->AddOnScreenDebugMessage(-1, deltaTime, FColor::Cyan, FString::Printf(TEXT("IS ESCAPING: %d"), escaping));
		}
		if (!hasWallInSight && (!playerFound || !isPlayerPowerUp)) {
			// Agent has successfully escaped from the corner
			escaping = Escaping::None;
		}
	}

	if (escaping != Escaping::None)
	{
		// Agent is currently escaping a corner
		//  -> Stop moving
		//  -> Rotate in the fastest estimated escape direction
		speed = 0.0f;
		walkingDirection = walkingDirection.Add(0, rotatingSpeed * escaping, 0);
	}
	else if (playerFound && !isPlayerPowerUp)
	{
		// Agent chases the detected player
		chaseObject(walkingDirection, playerLocation);
	} 
	else if (collectibleFound)
	{
		// Agent chases the detected collectible
		chaseObject(walkingDirection, collectibleLocation);
	}
	else
	{
		// Movement is not constraint by a higher proprity task
		freeRoam(speed, walkingDirection, centerHitResults, rightHitResults, leftHitResults, deltaTime);
	}

	// Stop the agent if death trap is found right in front of his feet
	if (sweepDeathTrap(feetCenter, boundingBox->GetScaledCapsuleRadius())) {
		speed = 0.f;
	}

	// Apply movement
	GetPawn()->SetActorRotation(walkingDirection);
	GetPawn()->AddMovementInput(walkingDirection.Vector(), speed);

	// Randomize envy
	envy = envy.Add(0, (rand() % 10) - 5, 0);
}

/*
* Name: findCollectible
* Description:
	Function that verify if a collectible is in the agent's detection sphere
	without being behind a wall and return that position if a collectible
	is found.
* Args:
	hit (FHitResult) : A collision hit result
	collectibleFound (bool&) : True if a collectible is found, else false
	collectibleLocation (FVector&) : collectible's location if collectible is found, else null
* Return: None
*/
void ASDTAIController::findCollectible(FHitResult hit, bool& collectibleFound, FVector& collectibleLocation) {
	struct FHitResult hitResult;
	FCollisionQueryParams params = FCollisionQueryParams();
	params.AddIgnoredActor(GetPawn());
	if (GetWorld()->LineTraceSingleByObjectType(hitResult, GetPawn()->GetActorLocation(), hit.GetActor()->GetActorLocation(), FCollisionObjectQueryParams::AllObjects, params)) {
		if (hitResult.GetComponent()->GetCollisionObjectType() == COLLISION_COLLECTIBLE) {
			if (hit.GetActor()->IsA(ASDTCollectible::StaticClass())) {
				ASDTCollectible* collectible = Cast<ASDTCollectible>(hit.GetActor());
				if (!collectible->IsOnCooldown()) {
					collectibleFound = true;
					collectibleLocation = hit.GetActor()->GetActorLocation();
				}
			}
		}
	}
}

/*
* Name: findPlayer
* Description:
	Function that verify if a player is in the agent's detection sphere
	without being behind a wall and return that position and player's
	power up status if a player is found.
* Args:
	hit (FHitResult) : A collision hit result
	playerFound (bool&) : True if a player is found, else false
	isPlayerPowerUp (bool&) : True if a player is powered up, else false
	playerLocation (FVector&) : player's location if player is found, else null
* Return: None
*/
void ASDTAIController::findPlayer(FHitResult hit, bool& playerFound, FVector& playerLocation, bool& isPlayerPowerUp) {
	struct FHitResult hitResult;
	FCollisionQueryParams params = FCollisionQueryParams();
	params.AddIgnoredActor(GetPawn());
	if (GetWorld()->LineTraceSingleByObjectType(hitResult, GetPawn()->GetActorLocation(), hit.GetActor()->GetActorLocation(), FCollisionObjectQueryParams::AllObjects, params)) {
		if (hitResult.GetComponent()->GetCollisionObjectType() == COLLISION_PLAYER) {
			if (ASoftDesignTrainingMainCharacter* player = Cast<ASoftDesignTrainingMainCharacter>(hit.GetActor())) {
				playerFound = true;
				playerLocation = hit.GetActor()->GetActorLocation();
				isPlayerPowerUp = player->IsPoweredUp();
			}
		}
	}
}

/*
* Name: freeRoam
* Description:
	Function that computes the speed and the rotation for the state where
	the agent roams freely while evading walls and obstacles (death traps).
* Args:
	speed (float&) : The agent's speed
	walkingDirection (FRotator&) : The agent's walking direction
	centerHitResults (TArray<struct FHitResult>) : List of collision hits by the center line trace
	rightHitResults (TArray<struct FHitResult>) : List of collision hits by the right most line trace
	leftHitResults (TArray<struct FHitResult>) : List of collision hits by the left most line trace
	deltaTime (float) : Time between two ticks.
* Return: None
*/
void ASDTAIController::freeRoam(float& speed, FRotator& walkingDirection, TArray<struct FHitResult> centerHitResults, TArray<struct FHitResult> rightHitResults, TArray<struct FHitResult> leftHitResults, float deltaTime) {

	// ***
	// SPEED AND TURNING COMPUTATION
	// ***
	float adjustedRotationSpeed = rotatingSpeed;
	if (rightHitResults.Num() + centerHitResults.Num() + leftHitResults.Num() > 0) {
		// Agent is seeing a wall, but is not in an escape sequence
		// -> Slowdown to prevent hitting the wall
		// -> Rotate to prevent hitting the wall

		if (centerHitResults.Num() > 0) {
			// Agent is seeing a wall right in front of him

			// Left wall collision detection
			float wallDistance;
			FVector_NetQuantizeNormal wallHitNormal;
			computeNearestCollision(wallDistance, wallHitNormal, centerHitResults);

			// Slowdown based on the wall distance from the front of the agent
			speed = wallDistance / sightThreshold;

			// Rotate slower the faster the agent is moving
			adjustedRotationSpeed = adjustedRotationSpeed / ((speed + 1) / 2);

			// Rotate
			FVector cross = FVector::CrossProduct(wallHitNormal, walkingDirection.Vector());
			walkingDirection = walkingDirection.Add(0, cross.Z > 0 ? -adjustedRotationSpeed : adjustedRotationSpeed, 0);

		}
		else {
			// Agent is seeing a wall from the corner of his eye

			// Rotate
			walkingDirection = walkingDirection.Add(0, rightHitResults.Num() > 0 ? -adjustedRotationSpeed : adjustedRotationSpeed, 0);
		}
	}
	else {
		// Agent is not seeing a wall.
		// -> Apply envy to the agents's rotation

		FVector cross = FVector::CrossProduct(envy.Vector(), walkingDirection.Vector());
		walkingDirection = walkingDirection.Add(0, cross.Z * envyStrength * adjustedRotationSpeed, 0);
	}
}

/*
* Name: computeNearestCollision
* Description:
	Function that returns the nearest collision in the given list of collision hits.
* Args:
	distance (float&) : Distance of the closest hit
	hitNormal (FVector_NetQuantizeNormal&) : Normal vector of the closest hit
	hits (TArray<struct FHitResult>) : List of collision hits
* Return: None
*/
void ASDTAIController::computeNearestCollision(float& distance, FVector_NetQuantizeNormal& hitNormal, TArray<struct FHitResult> hits) {
	distance = sightThreshold;
	for (FHitResult hit : hits) {
		if (hit.Distance < distance && hit.Component->GetCollisionObjectType() != ECollisionChannel::ECC_Pawn) {
			distance = hit.Distance;
			hitNormal = hit.ImpactNormal;
		}
	}
}//

/*
* Name: chaseObject
* Description:
	Function that computes the speed and rotation for a given agent that
	chases an object.
* Args:
	walkingDirection (FRotator&) : Agent's walking direction
	objectLocation (FVector) : The location of the object to chase after
* Return: None
*/
void ASDTAIController::chaseObject(FRotator& walkingDirection, FVector objectLocation) {
	float adjustedRotationSpeed = rotatingSpeed;
	FVector const objectVector = objectLocation - GetPawn()->GetActorLocation();
	FVector crossProduct = FVector::CrossProduct(GetPawn()->GetActorForwardVector(), objectVector);
	walkingDirection = walkingDirection.Add(0, crossProduct.Z > 0 ? adjustedRotationSpeed : -adjustedRotationSpeed, 0);
}

/*
* Name: sweepForwardObjects
* Description:
	Function that sweeps for all objects contained in a agent's detection sphere.
* Args:
	collectibleFound (bool&) : True if a collectible is found, else false
	collectibleLocation (FVector&) : collectible's location if collectible is found, else null
	playerFound (bool&) : True if a player is found, else false
	isPlayerPowerUp (bool&) : True if a player is powered up, else false
	playerLocation (FVector&) : player's location if player is found, else null
* Return: None
*/
void ASDTAIController::sweepForwardObjects(bool& collectibleFound, FVector& collectibleLocation, bool& playerFound, bool& isPlayerPowerUp, FVector& playerLocation) {
	TArray<struct FHitResult> HitResults;
	const FVector Start = GetPawn()->GetActorLocation() + GetPawn()->GetActorForwardVector() * detectionSphereRadius;
	const FVector End = GetPawn()->GetActorLocation() + GetPawn()->GetActorForwardVector() * detectionSphereRadius;
	const FCollisionShape SphereShape = FCollisionShape::MakeSphere(detectionSphereRadius);
	FCollisionQueryParams params = FCollisionQueryParams();
	params.AddIgnoredActor(GetPawn());

	GetWorld()->SweepMultiByObjectType(HitResults, Start, End, FQuat::Identity, FCollisionObjectQueryParams::AllObjects, SphereShape, params);
	if (debug) {
		DrawDebugSphere(GetWorld(), Start, detectionSphereRadius, 16, FColor::Yellow);
	}
	for (auto hit : HitResults) {
		if (hit.Component->GetCollisionObjectType() == COLLISION_COLLECTIBLE) {
			findCollectible(hit, collectibleFound, collectibleLocation);
		}
		if (hit.Component->GetCollisionObjectType() == COLLISION_PLAYER) {
			findPlayer(hit, playerFound, playerLocation, isPlayerPowerUp);
		}
	}
}

/*
* Name: sweepDeathTrap
* Description:
	Function that sweeps for all death traps contained in a agent's detection sphere.
* Args:
	pawnRadius (float&) : Radius of the pawn
	feetCenter (FVector&) : position of the pawn's feet
* Return: True if a death trap is found, else False
*/
bool ASDTAIController::sweepDeathTrap( FVector feetCenter, float pawnRadius  ) {
	TArray<struct FHitResult> HitResults;
	const FVector Start = feetCenter + GetPawn()->GetActorForwardVector() * pawnRadius * 0.5f;
	const FVector End = feetCenter + GetPawn()->GetActorForwardVector() * pawnRadius * 0.5f;
	const FCollisionShape SphereShape = FCollisionShape::MakeSphere(pawnRadius);
	FCollisionQueryParams params = FCollisionQueryParams();
	params.AddIgnoredActor(GetPawn());
	GetWorld()->SweepMultiByObjectType(HitResults, Start, End, FQuat::Identity, FCollisionObjectQueryParams::AllObjects, SphereShape, params);
	if (debug) {
		DrawDebugSphere(GetWorld(), Start, pawnRadius, 16, FColor::Yellow);
	}
	for (auto hit : HitResults) {
		if (hit.Component->GetCollisionObjectType() == COLLISION_DEATH_OBJECT) {
			return true;
		}
	}
	return false;
}