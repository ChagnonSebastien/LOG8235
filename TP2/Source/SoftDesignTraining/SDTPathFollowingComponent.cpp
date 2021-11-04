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

void USDTPathFollowingComponent::SetMoveSegment(int32 segmentStartIndex)
{
    Super::SetMoveSegment(segmentStartIndex);

    const TArray<FNavPathPoint>& points = Path->GetPathPoints();
    const FNavPathPoint& segmentStart = points[MoveSegmentStartIndex];
    const FNavPathPoint& segmentEnd = points[DetermineCurrentTargetPathPoint(MoveSegmentStartIndex)];

    ASDTAIController* controller = Cast<ASDTAIController>(GetOwner());

    if (FNavMeshNodeFlags(segmentStart.Flags).IsNavLink())
    {
        // Handle starting jump
        controller->AtJumpSegment = true;
        controller->Landing = false;

        FVector2D startVector(segmentStart.Location);
        FVector2D currentVector(controller->GetPawn()->GetActorLocation());
        FVector2D endVector(segmentEnd.Location);

        float totalDistance = FVector2D::Distance(startVector, endVector);
        float currentDistance = FVector2D::Distance(startVector, currentVector);
        controller->SetJumpDistance(currentDistance / totalDistance);
    }
    else
    {
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