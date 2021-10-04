// Fill out your copyright notice in the Description page of Project Settings.

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

	//bool isPlayerNearby = false;
	//bool isPoweredUpPlayerNearby = false;
	//FVector nearbyPlayerLocation;

	//sweepNearbyObjects(isPlayerNearby, isPoweredUpPlayerNearby, nearbyPlayerLocation);

	bool collectibleFound = false;
	FVector collectibleLocation;
	bool playerFound = false;
	bool isPlayerPowerUp = false;
	FVector playerLocation;

	sweepFowardObjects(collectibleFound, collectibleLocation, playerFound, isPlayerPowerUp, playerLocation);

	float speed = 1.0f;
	FRotator walkingDirection = GetPawn()->GetActorRotation();

	// ***
	// CORNER DETECTION
	// ***
	if (escapingCorner == 0) {
		// Agent is not currently escaping from a corner

		if (playerFound && isPlayerPowerUp)
		{
			FVector const objectVector = playerLocation - GetPawn()->GetActorLocation();
			FVector crossProduct = FVector::CrossProduct(GetPawn()->GetActorForwardVector(), objectVector);
			escapingCorner = crossProduct.Z > 0 ? -1 : 1;
		}
		else if (rightHitResults.Num() > 0 && leftHitResults.Num() > 0)
		{
			// Left wall collision detection
			float leftWallDistance;
			FVector_NetQuantizeNormal leftWallHitNormal;
			computeNeasestCollision(leftWallDistance, leftWallHitNormal, leftHitResults);

			// Right wall collision detection
			float rightWallDistance;
			FVector_NetQuantizeNormal rightWallHitNormal;
			computeNeasestCollision(rightWallDistance, rightWallHitNormal, rightHitResults);

			if (!(leftWallHitNormal - rightWallHitNormal).IsNearlyZero()) {
				// Both sides of the agent collision detection are not hitting the same surface normal. Therefore, the agent must be in front of a corner.
				escapingCorner = (leftWallDistance > rightWallDistance) ? -1 : 1;
			}
		}
	}
	else {
		// Agent is currently escaping from a corner

		if (debug) {
			GEngine->AddOnScreenDebugMessage(-1, deltaTime, FColor::Cyan, FString::Printf(TEXT("IS ESCAPING: %f"), escapingCorner));
		}
		if (!hasWallInSight && (!playerFound || !isPlayerPowerUp)) {
			// Agent has successfully escaped from the corner
			escapingCorner = 0;
		}
	}

	if (escapingCorner != 0)
	{
		// Agent is currently escaping a corner
		//  -> Stop moving
		//  -> Rotate in the fastest estimated escape direction
		speed = 0.0f;
		walkingDirection = walkingDirection.Add(0, rotatingSpeed * escapingCorner, 0);
	}
	else if (playerFound && !isPlayerPowerUp)
	{
		chaseObject(walkingDirection, playerLocation);
	} 
	else if (collectibleFound)
	{
		chaseObject(walkingDirection, collectibleLocation);
	}
	else
	{
		// Movement is not constraint by a higher proprity task
		freeRoam(speed, walkingDirection, centerHitResults, rightHitResults, leftHitResults, deltaTime);
	}

	// Apply movement
	GetPawn()->SetActorRotation(walkingDirection);
	GetPawn()->AddMovementInput(walkingDirection.Vector(), speed);

	// Randomize envy
	envy = envy.Add(0, (rand() % 10) - 5, 0);
}

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
			computeNeasestCollision(wallDistance, wallHitNormal, centerHitResults);

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

void ASDTAIController::computeNeasestCollision(float &distance, FVector_NetQuantizeNormal &hitNormal, TArray<struct FHitResult> hits) {
	distance = sightThreshold;
	for (FHitResult hit : hits) {
		if (hit.Distance < distance) {
			distance = hit.Distance;
			hitNormal = hit.ImpactNormal;
		}
	}
}

void ASDTAIController::chaseObject(FRotator& walkingDirection, FVector objectLocation) {
	float adjustedRotationSpeed = rotatingSpeed;
	FVector const objectVector = objectLocation - GetPawn()->GetActorLocation();
	FVector crossProduct = FVector::CrossProduct(GetPawn()->GetActorForwardVector(), objectVector);
	walkingDirection = walkingDirection.Add(0, crossProduct.Z > 0 ? adjustedRotationSpeed : -adjustedRotationSpeed, 0);
}
//
//void ASDTAIController::runFromObject(float& speed, FRotator& walkingDirection, FVector objectLocation) {
//	speed = 0.f;
//	float adjustedRotationSpeed = 5.f;
//	FVector const objectVector = objectLocation - GetPawn()->GetActorLocation();
//	FVector crossProduct = FVector::CrossProduct(GetPawn()->GetActorForwardVector(), objectVector);
//	walkingDirection = walkingDirection.Add(0, crossProduct.Z > 0 ? -adjustedRotationSpeed : adjustedRotationSpeed, 0);
//}
//
//void ASDTAIController::sweepNearbyObjects(bool& isPlayerNearby, bool& isPoweredUpPlayerNearby, FVector& nearbyPlayerLocation) {
//	TArray<struct FHitResult> nearbyHitResults;
//	const FCollisionShape fleeSphereShape = FCollisionShape::MakeSphere(400.f);
//	FCollisionQueryParams params = FCollisionQueryParams();
//	params.AddIgnoredActor(GetPawn());
//
//	GetWorld()->SweepMultiByObjectType(nearbyHitResults, GetPawn()->GetActorLocation(), GetPawn()->GetActorLocation(), FQuat::Identity, FCollisionObjectQueryParams::AllObjects, fleeSphereShape, params);
//	if (debug) {
//		DrawDebugSphere(GetWorld(), GetPawn()->GetActorLocation(), 400.f, 16, FColor::Orange);
//	}
//	for (auto nearbyHit : nearbyHitResults) {
//		if (nearbyHit.Component->GetCollisionObjectType() == COLLISION_PLAYER) {
//			findPlayer(nearbyHit, isPlayerNearby, nearbyPlayerLocation, isPoweredUpPlayerNearby);
//		}
//	}
//}

void ASDTAIController::sweepFowardObjects(bool& collectibleFound, FVector& collectibleLocation, bool& playerFound, bool& isPlayerPowerUp, FVector& playerLocation) {
	TArray<struct FHitResult> HitResults;
	const FVector Start = GetPawn()->GetActorLocation() + GetPawn()->GetActorForwardVector() * 200.f;
	const FVector End = GetPawn()->GetActorLocation() + GetPawn()->GetActorForwardVector() * 200.f;
	const FCollisionShape SphereShape = FCollisionShape::MakeSphere(200.f);
	FCollisionQueryParams params = FCollisionQueryParams();
	params.AddIgnoredActor(GetPawn());

	GetWorld()->SweepMultiByObjectType(HitResults, Start, End, FQuat::Identity, FCollisionObjectQueryParams::AllObjects, SphereShape, params);
	if (debug) {
		DrawDebugSphere(GetWorld(), Start, 200.f, 16, FColor::Yellow);
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
//
//void ASDTAIController::UpdatePawnIsEscapingPlayerStatus(bool& isPoweredUpPlayerNearby, const FVector nearbyPlayerLocation) {
//	if (isPoweredUpPlayerNearby && isEscapingPoweredUpPlayer) {
//		// If angle between foward vector and pawn to player vector is > 170
//		// set isEscapingPoweredUpPlayer to false
//		FVector pawnToPlayer = nearbyPlayerLocation - GetPawn()->GetActorLocation();
//		float angle = std::abs(std::acos(FVector::DotProduct(pawnToPlayer.GetSafeNormal(), GetPawn()->GetActorForwardVector().GetSafeNormal()))) * 180 / PI;
//		if (angle > 170 && !SDTUtils::Raycast(GetWorld(), GetPawn()->GetActorLocation(), GetPawn()->GetActorLocation() + (GetPawn()->GetActorForwardVector() * 100.f))) {
//			isEscapingPoweredUpPlayer = false;
//		}
//	}
//	else {
//		isEscapingPoweredUpPlayer = false;
//	}
//}