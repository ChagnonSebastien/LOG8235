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
	FVector const feetCenter = GetPawn()->GetActorLocation() + feetOffset + FVector(0, 0, 1);

	UCapsuleComponent* const boundingBox = (UCapsuleComponent*)GetPawn()->GetComponentByClass(UCapsuleComponent::StaticClass());
	FVector const rightFoot = feetCenter + (GetPawn()->GetActorRightVector().GetSafeNormal() * boundingBox->GetScaledCapsuleRadius());
	FVector const leftFoot = feetCenter + (GetPawn()->GetActorRightVector().GetSafeNormal() * boundingBox->GetScaledCapsuleRadius() * -1);

	auto const sight = (GetPawn()->GetActorRotation().Vector().GetSafeNormal() * SIGHT_THRESHOLD);
	auto const rightFootSight = rightFoot + sight / 2;
	auto const centerSight = feetCenter + sight;
	auto const leftFootSight = leftFoot + sight / 2;

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

	int totalHits = rightHitResults.Num() + centerHitResults.Num() + leftHitResults.Num();

	if (debug) {
		GEngine->AddOnScreenDebugMessage(-1, deltaTime, FColor::Red, FString::Printf(TEXT("%d"), totalHits));
	}

	auto const  hasWallInSight = totalHits > 0;

	FRotator walkingDirection = GetPawn()->GetActorRotation();

	if (hasWallInSight) {
		//slowDown()
		
		//turn()
		walkingDirection = walkingDirection.Add(0, 3, 0);
	}
	else {
		//restet speed to normal
	}

	GetPawn()->SetActorRotation(walkingDirection);
	GetPawn()->AddMovementInput(walkingDirection.Vector());
}

