// Fill out your copyright notice in the Description page of Project Settings.

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
        controller->GetPawn()->SetActorRotation(FMath::Lerp(controller->GetPawn()->GetActorRotation(), orientation.Rotation(), 0.055f));
        
        // Update AI agent location based on orientation and movement speed
        FVector newLocation;
        bool isNextSegmentJump = IsSegmentNavigationLink(MoveSegmentEndIndex);
        bool isCloseToNextSegment = controller->m_MovementSpeed * DeltaTime * 100 > FVector::Dist2D(currentLocation, segmentEnd);
        bool isVeryCloseToNextSegment = controller->m_MovementSpeed * DeltaTime * 10 > FVector::Dist2D(currentLocation, segmentEnd);
        bool isJumping = IsSegmentNavigationLink(MoveSegmentStartIndex);
        FVector currentRotation = controller->GetPawn()->GetActorRotation().Vector();
        bool isFacingGoal = FVector2D::DotProduct(FVector2D(currentLocation), FVector2D(orientation)) > 0;
        bool isAbountToJump = isNextSegmentJump && (isVeryCloseToNextSegment || isCloseToNextSegment && isFacingGoal);
        if (isAbountToJump || isJumping)
        {
            newLocation = currentLocation + controller->m_MovementSpeed * DeltaTime * orientation;
        }
        else
        {
            newLocation = currentLocation + controller->m_MovementSpeed * DeltaTime * currentRotation;
        }
        controller->GetPawn()->SetActorLocation(newLocation);
        controller->CloseToJumpSegment = isAbountToJump;
    }
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
        
        CalculateJumpDistance(segmentStart.Location, segmentEnd.Location);
        if (FVector::Dist2D(pawnLocation, FVector(segmentEnd)) > 75) {
            // Handle starting jump
            controller->AtJumpSegment = true;
            controller->Landing = false;
        }
        else if (controller->InAir) {
            // Handle landing
            controller->AtJumpSegment = false;
            controller->Landing = true;
        }
    }
    else {
        // Handle normal segments
        controller->AtJumpSegment = false;
        controller->Landing = true;
        controller->SetJumpDistance(1);
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

/**
* Function that verify if a segment is a navigation link from the segment's index.
*
* @param segmentStartIndex The start index of the segment to check.
* 
* @return `true` if the segment is a navigation link, else `false`.
*/
bool USDTPathFollowingComponent::IsSegmentNavigationLink(int32 segmentStartIndex)
{
    const TArray<FNavPathPoint>& points = Path->GetPathPoints();
    if (segmentStartIndex + 1 >= points.Num())
    {
        return false;
    }
    const FNavPathPoint& segmentStart = points[segmentStartIndex];
    return FNavMeshNodeFlags(segmentStart.Flags).IsNavLink();
}

/**
* Function that calculate and set the z value of the pawn when he is InAir.
*
* @param startLocation Location of the starting point of the jump.
* @param endLocation   Location of the ending point of the jump.
*/
void USDTPathFollowingComponent::CalculateJumpDistance(FVector startLocation, FVector endLocation)
{
    ASDTAIController* controller = Cast<ASDTAIController>(GetOwner());
    FVector pawnLocation(controller->GetPawn()->GetActorLocation());

    float totalDistance = FVector::Dist2D(startLocation, endLocation);
    float currentDistance = FVector::Dist2D(startLocation, pawnLocation);
    controller->SetJumpDistance(currentDistance / totalDistance);
}
