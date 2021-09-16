// Fill out your copyright notice in the Description page of Project Settings.

#include "SDTAIController.h"
#include "SoftDesignTraining.h"
#include "SDTUtils.h"

void ASDTAIController::Tick(float deltaTime)
{
	UWorld* world = GetWorld();
	auto const actorLocation = GetPawn()->GetActorLocation();
	auto const  hasWall = SDTUtils::Raycast(world, actorLocation, actorLocation + GetPawn()->GetActorRotation().Vector().GetSafeNormal()* DISTANCE_THRESHOLD);
	if (hasWall) {
		//slowDown()
		
		//turn()
		GetPawn()->SetActorRotation(GetPawn()->GetActorRotation().Add(0, 100, 0));
	}
	else {
		//restet speed to normal
	}
	auto const orientation2D = FVector2D(GetPawn()->GetActorRotation().Vector());
	MoveTowardsDirection(orientation2D, 200, deltaTime, 500);
}

void ASDTAIController::MoveTowardsDirection(FVector2D direction, float speed, float deltaTime, float maxSpeed)
{
	APawn* pawn = GetPawn();
	FVector const pawnPosition(pawn->GetActorLocation());
	FVector2D const displacement = FMath::Min(maxSpeed, speed) * deltaTime * direction.GetSafeNormal();
	pawn->SetActorLocation(pawnPosition + FVector(displacement, 0.f), true);
	pawn->SetActorRotation(FVector(direction, 0.f).ToOrientationQuat());
}

