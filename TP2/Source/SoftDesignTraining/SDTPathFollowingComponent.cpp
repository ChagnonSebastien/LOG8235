// Fill out your copyright notice in the Description page of Project Settings.
/*
 Authors:
   - Sébastien Chagnon (1804702)
   - Andy Lam (1960209)
   - Jason Thai (1956172)
   - Alexandre Ramtoula (1959456)
   - Philippe Trempe (1686069)
*/
#include "SDTPathFollowingComponent.h"
#include "SoftDesignTraining.h"
#include "SDTUtils.h"
#include "SDTAIController.h"
#include "GameFramework/CharacterMovementComponent.h"

#include "DrawDebugHelpers.h"

/**
* Constructs an USDTPathFollowingComponent from an FObjectInitializer.
*
* @param ObjectInitializer The initializer to use to construct the USDTPathFollowingComponent.
*/
USDTPathFollowingComponent::USDTPathFollowingComponent(const FObjectInitializer& ObjectInitializer)
{

}

/**
* Updates the path following logic by applying a time tick.
*
* @param deltaTime The tick delta time.
*/
void USDTPathFollowingComponent::FollowPathSegment(float DeltaTime)
{
    const TArray<FNavPathPoint>& points = Path->GetPathPoints();
    const FNavPathPoint& segmentStart = points[MoveSegmentStartIndex];
    const FNavPathPoint& segmentEnd = points[DetermineCurrentTargetPathPoint(MoveSegmentStartIndex)];

    ASDTAIController* controller = Cast<ASDTAIController>(GetOwner());
    if (controller->AtJumpSegment) return;

    // Compute AI agent orientation
    FVector currentLocation = controller->GetPawn()->GetActorLocation();
    FVector orientation = segmentEnd.Location - segmentStart.Location;
    orientation.Normalize();
        
    // Smoothly update AI agent rotation based on orientation
    FQuat rotation = FRotationMatrix::MakeFromXZ(orientation, FVector::UpVector).ToQuat();
    controller->GetPawn()->SetActorRotation(FMath::Lerp(controller->GetPawn()->GetActorRotation(), orientation.Rotation(), 0.055f));
        
    // Find the closest JumpSegment;
    float closestDistance = 1000000.0f;
    FVector ClosestJumpSegment = FVector(1000000, 1000000, 0);
    for (int i = MoveSegmentEndIndex; i < points.Num() - 1; i++) {
        const FNavPathPoint& point = points[i];
        float distance = FVector2D::Distance(FVector2D(point.Location), FVector2D(currentLocation));
        if (FNavMeshNodeFlags(point.Flags).IsNavLink() && distance < closestDistance) {
            closestDistance = distance;
            ClosestJumpSegment = point.Location;
        }
    }

    // Update AI agent location based on orientation and movement speed
    FVector newLocation;
    bool isCloseToNextSegment = controller->m_MovementSpeed * DeltaTime * 100 > closestDistance;
    bool isVeryCloseToNextSegment = controller->m_MovementSpeed * DeltaTime * 10 > closestDistance;
    FVector currentRotation = controller->GetPawn()->GetActorRotation().Vector();
    bool isFacingGoal = FVector2D::DotProduct(FVector2D(orientation), FVector2D(ClosestJumpSegment - currentLocation)) > 0;
    bool isAboutToJump = isCloseToNextSegment && isFacingGoal;
    if (isAboutToJump)
    {
        newLocation = currentLocation + controller->m_MovementSpeed * DeltaTime * orientation;
    }
    else
    {
        newLocation = currentLocation + controller->m_MovementSpeed * DeltaTime * currentRotation;
    }
    controller->GetPawn()->SetActorLocation(newLocation);
    controller->CloseToJumpSegment = isAboutToJump;
}

/**
* Sets the current segment to move through.
*
* @param segmentStartIndex The segment start index.
*/
void USDTPathFollowingComponent::SetMoveSegment(int32 segmentStartIndex)
{
    Super::SetMoveSegment(segmentStartIndex);

    const TArray<FNavPathPoint>& points = Path->GetPathPoints();
    const FNavPathPoint& segmentStart = points[MoveSegmentStartIndex];
    const FNavPathPoint& segmentEnd = points[MoveSegmentEndIndex];

    ASDTAIController* controller = Cast<ASDTAIController>(GetOwner());
    FVector pawnLocation(controller->GetPawn()->GetActorLocation());
    
    if (FNavMeshNodeFlags(segmentStart.Flags).IsNavLink()) {
        controller->Jump(segmentEnd);
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
