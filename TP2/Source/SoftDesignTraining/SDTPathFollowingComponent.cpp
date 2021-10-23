// Fill out your copyright notice in the Description page of Project Settings.

#include "SDTPathFollowingComponent.h"
#include "SoftDesignTraining.h"
#include "SDTUtils.h"
#include "SDTAIController.h"
#include "GameFramework/CharacterMovementComponent.h"

#include "DrawDebugHelpers.h"

USDTPathFollowingComponent::USDTPathFollowingComponent(const FObjectInitializer& ObjectInitializer)
{

}

void USDTPathFollowingComponent::FollowPathSegment(float DeltaTime)
{
    const TArray<FNavPathPoint>& points = Path->GetPathPoints();
    const FNavPathPoint& segmentStart = points[MoveSegmentStartIndex];
    const FNavPathPoint& segmentEnd = points[DetermineCurrentTargetPathPoint(MoveSegmentStartIndex)];

    ASDTAIController* controller = Cast<ASDTAIController>(GetOwner());

    if (SDTUtils::HasJumpFlag(segmentStart))
    {
        // Update jump
    }
    else
    {
        // Update navigation along path

        // Compute AI agent orientation
        FVector currentLocation = controller->GetPawn()->GetActorLocation();
        FVector orientation = segmentEnd.Location - segmentStart.Location;
        orientation.Normalize();
        
        // Smoothly update AI agent rotation based on orientation
        FQuat rotation = FRotationMatrix::MakeFromXZ(orientation, FVector::UpVector).ToQuat();
        controller->GetPawn()->SetActorRotation(FMath::Lerp(controller->GetPawn()->GetActorRotation(), orientation.Rotation(), 0.05f));
        
        // Update AI agent location based on orientation and movement speed
        FVector newLocation = currentLocation + controller->m_MovementSpeed * DeltaTime * orientation;
        controller->GetPawn()->SetActorLocation(newLocation);
    }
}

void USDTPathFollowingComponent::SetMoveSegment(int32 segmentStartIndex)
{
    Super::SetMoveSegment(segmentStartIndex);

    const TArray<FNavPathPoint>& points = Path->GetPathPoints();
    const FNavPathPoint& segmentStart = points[MoveSegmentStartIndex];
    const FNavPathPoint& segmentEnd = points[DetermineCurrentTargetPathPoint(MoveSegmentStartIndex)];

    ASDTAIController* controller = Cast<ASDTAIController>(GetOwner());

    if (SDTUtils::HasJumpFlag(segmentStart) && FNavMeshNodeFlags(segmentStart.Flags).IsNavLink())
    {
        // Handle starting jump
        controller->AtJumpSegment = true;
    }
    else
    {
        // Handle normal segments
        controller->AtJumpSegment = false;
    }

    FColor color;
    if (controller->AtJumpSegment) {
        color = FColor(0, 255, 0);
    }
    else {
        color = FColor(0, 0, 255);
    }
    DrawDebugSphere(GetWorld(), controller->GetPawn()->GetActorLocation() + FVector::UpVector * 120.0f, 10.0f, 32, color);
}
