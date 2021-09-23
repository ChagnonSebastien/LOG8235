// Fill out your copyright notice in the Description page of Project Settings.

#include "SDTAIController.h"
#include "SoftDesignTraining.h"
#include "DrawDebugHelpers.h"
#include "SDTUtils.h"

void ASDTAIController::Tick(float deltaTime)
{
	UWorld* world = GetWorld();
	UMeshComponent* const playerMesh = (UMeshComponent*) GetPawn()->GetComponentByClass(UMeshComponent::StaticClass());
	FVector const feetOffset = playerMesh->GetRelativeLocation();
	FVector const actorLocation = GetPawn()->GetActorLocation() + feetOffset;
	auto const eyeSight = actorLocation + (GetPawn()->GetActorRotation().Vector().GetSafeNormal() * DISTANCE_THRESHOLD);
	if (debug) {
		// GEngine->AddOnScreenDebugMessage(-1, 1, FColor::Red, FString::Printf(TEXT("(%d, %d, %d)"), feetOffset.X, feetOffset.Y, feetOffset.Z));
		DrawDebugLine(world, actorLocation, eyeSight, FColor::Red);
	}

	auto const  hasWallInSight = SDTUtils::Raycast(world, actorLocation, eyeSight);

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

void ASDTAIController::MoveTowardsDirection(FVector2D direction, float speed, float deltaTime, float maxSpeed)
{
	APawn* pawn = GetPawn();
	FVector const pawnPosition(pawn->GetActorLocation());
	FVector2D const displacement = FMath::Min(maxSpeed, speed) * deltaTime * direction.GetSafeNormal();
	pawn->SetActorLocation(pawnPosition + FVector(displacement, 0.f), true);
	pawn->SetActorRotation(FVector(direction, 0.f).ToOrientationQuat());
}

