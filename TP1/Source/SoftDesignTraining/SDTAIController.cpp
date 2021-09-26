// Fill out your copyright notice in the Description page of Project Settings.

#include "SDTAIController.h"
#include "SoftDesignTraining.h"
#include "DrawDebugHelpers.h"
#include "SDTUtils.h"

void ASDTAIController::Tick(float deltaTime)
{
	UWorld* world = GetWorld();
	UMeshComponent* const characterMesh = (UMeshComponent*) GetPawn()->GetComponentByClass(UMeshComponent::StaticClass());
	FVector const feetOffset = characterMesh->GetRelativeLocation();
	FVector const feetCenter = GetPawn()->GetActorLocation() + feetOffset + FVector(0, 0, 2);

	UCapsuleComponent* const boundingBox = (UCapsuleComponent*)GetPawn()->GetComponentByClass(UCapsuleComponent::StaticClass());
	FVector const rightFoot = feetCenter + (GetPawn()->GetActorRightVector().GetSafeNormal() * boundingBox->GetScaledCapsuleRadius());
	FVector const leftFoot = feetCenter + (GetPawn()->GetActorRightVector().GetSafeNormal() * boundingBox->GetScaledCapsuleRadius() * -1);

	FVector const sight = (GetPawn()->GetActorRotation().Vector().GetSafeNormal() * SIGHT_THRESHOLD);
	FVector const rightFootSight = rightFoot + sight / 2;
	FVector const centerSight = feetCenter + sight;
	FVector const leftFootSight = leftFoot + sight / 2;

	if (debug) {
		DrawDebugLine(world, rightFoot, rightFootSight, FColor::Red);
		DrawDebugLine(world, feetCenter, centerSight, FColor::Red);
		DrawDebugLine(world, leftFoot, leftFootSight, FColor::Red);
	}

	FCollisionQueryParams params = FCollisionQueryParams();
	params.AddIgnoredActor(GetPawn());

	TArray<struct FHitResult> rightHitResults;
	TArray<struct FHitResult> centerHitResults;
	TArray<struct FHitResult> leftHitResults;

	world->LineTraceMultiByObjectType(rightHitResults, rightFoot, rightFootSight, FCollisionObjectQueryParams::AllObjects, params);
	world->LineTraceMultiByObjectType(centerHitResults, feetCenter, centerSight, FCollisionObjectQueryParams::AllObjects, params);
	world->LineTraceMultiByObjectType(leftHitResults, leftFoot, leftFootSight, FCollisionObjectQueryParams::AllObjects, params);

	bool const hasWallInSight = rightHitResults.Num() + centerHitResults.Num() + leftHitResults.Num() > 0;

	FRotator walkingDirection = GetPawn()->GetActorRotation();

	// ***
	// CORNER DETECTION
	// ***
	if (escapingCorner == 0) {
		if (rightHitResults.Num() > 0 && leftHitResults.Num() > 0) {
			float leftWallDistance;
			FVector_NetQuantizeNormal leftWallHitNormal;
			computeNeasestCollision(leftWallDistance, leftWallHitNormal, leftHitResults);

			float rightWallDistance;
			FVector_NetQuantizeNormal rightWallHitNormal;
			computeNeasestCollision(rightWallDistance, rightWallHitNormal, rightHitResults);

			if (!(leftWallHitNormal - rightWallHitNormal).IsNearlyZero()) {
				escapingCorner = (leftWallDistance > rightWallDistance) ? -1 : 1;

				GEngine->AddOnScreenDebugMessage(-1, 10, FColor::Yellow, FString::Printf(TEXT("%f"), escapingCorner));
				GEngine->AddOnScreenDebugMessage(-1, 1, FColor::Blue, FString::Printf(TEXT("ESCAPING")));
			}
		}
	}
	else {
		GEngine->AddOnScreenDebugMessage(-1, deltaTime, FColor::Cyan, FString::Printf(TEXT("IS ESCAPING")));
		if (!hasWallInSight) {
			escapingCorner = 0;
		}
	}

	// ***
	// SPEED AND TURNING COMPUTATION
	// ***
	float speed = 1.0f;
	float adjustedRotationSpeed = ROTATING_SPEED;
	if (escapingCorner != 0) {
		speed = 0.0f;
		adjustedRotationSpeed = adjustedRotationSpeed * escapingCorner;
		walkingDirection = walkingDirection.Add(0, adjustedRotationSpeed, 0);
	}
	else if (hasWallInSight) {
		if (centerHitResults.Num() > 0) {

			float wallDistance;
			FVector_NetQuantizeNormal wallHitNormal;
			computeNeasestCollision(wallDistance, wallHitNormal, centerHitResults);

			// Slowdown to prevent hitting wall
			speed = wallDistance / SIGHT_THRESHOLD;
			adjustedRotationSpeed = adjustedRotationSpeed / ((speed + 1) / 2);

			// Turn
			FVector cross = FVector::CrossProduct(wallHitNormal, walkingDirection.Vector());
			walkingDirection = walkingDirection.Add(0, cross.Z > 0 ? -adjustedRotationSpeed : adjustedRotationSpeed, 0);

		}
		else {

			//turn()
			walkingDirection = walkingDirection.Add(0, rightHitResults.Num() > 0 ? -adjustedRotationSpeed : adjustedRotationSpeed, 0);
		}
	}
	else {

		FVector cross = FVector::CrossProduct(envy.Vector(), walkingDirection.Vector());
		walkingDirection = walkingDirection.Add(0, cross.Z * ENVY_MAGNITUDE * adjustedRotationSpeed, 0);
	}

	GetPawn()->SetActorRotation(walkingDirection);
	GetPawn()->AddMovementInput(walkingDirection.Vector(), speed);

	envy = envy.Add(0, (rand() % 10) - 5, 0);
}

void ASDTAIController::computeNeasestCollision(float &distance, FVector_NetQuantizeNormal &hitNormal, TArray<struct FHitResult> hits) {
	distance = SIGHT_THRESHOLD;
	for (FHitResult hit : hits) {
		if (hit.Distance < distance) {
			distance = hit.Distance;
			hitNormal = hit.ImpactNormal;
		}
	}
}