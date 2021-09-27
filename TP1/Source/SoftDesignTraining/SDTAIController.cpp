// Fill out your copyright notice in the Description page of Project Settings.

#include "SDTAIController.h"
#include "SoftDesignTraining.h"
#include "DrawDebugHelpers.h"
#include "SDTUtils.h"

void ASDTAIController::Tick(float deltaTime)
{
	// Compute agents's feet position
	UMeshComponent* const characterMesh = (UMeshComponent*) GetPawn()->GetComponentByClass(UMeshComponent::StaticClass());
	FVector const feetOffset = characterMesh->GetRelativeLocation();
	FVector const feetCenter = GetPawn()->GetActorLocation() + feetOffset + FVector(0, 0, 2);

	float speed = 1.0f;
	FRotator walkingDirection = GetPawn()->GetActorRotation();
	if (false) {
		// Agent is seeing a collectible, player, or death trap

	}
	else {
		// Movement is not constraint by a higher proprity task
		freeRoam(speed, walkingDirection, feetCenter, deltaTime);
	}

	// Apply movement
	GetPawn()->SetActorRotation(walkingDirection);
	GetPawn()->AddMovementInput(walkingDirection.Vector(), speed);

	// Randomize envy
	envy = envy.Add(0, (rand() % 10) - 5, 0);
}

void ASDTAIController::freeRoam(float& speed, FRotator& walkingDirection, FVector feetCenter, float deltaTime) {
	UWorld* world = GetWorld();

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

	// ***
	// CORNER DETECTION
	// ***
	if (escapingCorner == 0) {
		// Agent is not currently escaping from a corner

		if (rightHitResults.Num() > 0 && leftHitResults.Num() > 0) {
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

				if (debug) {
					GEngine->AddOnScreenDebugMessage(-1, 5, FColor::Yellow, FString::Printf(TEXT("ESCAPING CORNER: %f"), escapingCorner));
				}
			}
		}
	}
	else {
		// Agent is currently escaping from a corner

		if (debug) {
			GEngine->AddOnScreenDebugMessage(-1, deltaTime, FColor::Cyan, FString::Printf(TEXT("IS ESCAPING")));
		}
		if (!hasWallInSight) {
			// Agent has successfully escaped from the corner
			escapingCorner = 0;
		}
	}

	// ***
	// SPEED AND TURNING COMPUTATION
	// ***
	float adjustedRotationSpeed = rotatingSpeed;
	if (escapingCorner != 0) {
		// Agent is currently escaping a corner
		//  -> Stop moving
		//  -> Rotate in the fastest estimated escape direction
		speed = 0.0f;
		adjustedRotationSpeed = adjustedRotationSpeed * escapingCorner;
		walkingDirection = walkingDirection.Add(0, adjustedRotationSpeed, 0);
	}
	else if (hasWallInSight) {
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