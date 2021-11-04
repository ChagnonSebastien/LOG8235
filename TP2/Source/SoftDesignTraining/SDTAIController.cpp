// Fill out your copyright notice in the Description page of Project Settings.

#include "NavigationSystem.h"
#include "SDTAIController.h"
#include "SoftDesignTraining.h"
#include "SDTCollectible.h"
#include "SDTFleeLocation.h"
#include "SDTPathFollowingComponent.h"
#include "SoftDesignTrainingMainCharacter.h"
#include "DrawDebugHelpers.h"
#include "Kismet/KismetMathLibrary.h"
#include "Kismet/GameplayStatics.h"
//#include "UnrealMathUtility.h"
#include <cmath>  // For fleeing decision

#include "SDTUtils.h"
#include "EngineUtils.h"
#include "SDTCollectible.h"

ASDTAIController::ASDTAIController(const FObjectInitializer& ObjectInitializer)
    : Super(ObjectInitializer.SetDefaultSubobjectClass<USDTPathFollowingComponent>(TEXT("PathFollowingComponent")))
{
}

void ASDTAIController::GoToBestTarget(float deltaTime)
{
    UpdateSpeed();

    // Update path follower with new path
    GetPathFollowingComponent()->RequestMove(FAIMoveRequest(), m_pathToTarget->GetPath());


}

void ASDTAIController::UpdateSpeed() {
    if (AtJumpSegment || CloseToJumpSegment)
    {
        m_NewSpeed = m_RunningSpeed * JumpSpeed;
    }
    else if (fleeing || playerFound)
    {
        m_NewSpeed = m_RunningSpeed;
    }
    else
    {
        m_NewSpeed = m_WalkingSpeed;
    }

    if (m_NewSpeed < m_MovementSpeed)
    {
        m_MovementSpeed -= 20.f;
    }
    else if (m_NewSpeed > m_MovementSpeed)
    {
        m_MovementSpeed += 10.f;
    }
}

void ASDTAIController::OnMoveToTarget()
{
    m_ReachedTarget = false;
}

void ASDTAIController::OnMoveCompleted(FAIRequestID RequestID, const FPathFollowingResult& Result)
{
    Super::OnMoveCompleted(RequestID, Result);

    m_ReachedTarget = true;
}

void ASDTAIController::ChooseBehavior(float deltaTime)
{
    UpdatePlayerInteraction(deltaTime);

    ShowNavigationPath();
}

void ASDTAIController::UpdatePlayerInteraction(float deltaTime)
{
    //finish jump before updating AI state
    if (AtJumpSegment)
        return;

    APawn* selfPawn = GetPawn();
    if (!selfPawn)
        return;

    FVector detectionStartLocation = selfPawn->GetActorLocation() + selfPawn->GetActorForwardVector() * m_DetectionCapsuleForwardStartingOffset;
    FVector detectionEndLocation = detectionStartLocation + selfPawn->GetActorForwardVector() * m_DetectionCapsuleHalfLength * 2;

    TArray<TEnumAsByte<EObjectTypeQuery>> detectionQueryParams;
    detectionQueryParams.Add(UEngineTypes::ConvertToObjectType(COLLISION_COLLECTIBLE));
    detectionQueryParams.Add(UEngineTypes::ConvertToObjectType(COLLISION_PLAYER));

    TArray<FHitResult> allDetectionHits;
    GetWorld()->SweepMultiByObjectType(allDetectionHits, detectionStartLocation, detectionEndLocation, FQuat::Identity, detectionQueryParams, FCollisionShape::MakeSphere(m_DetectionCapsuleRadius));

    FHitResult detectionHit;
    GetHightestPriorityDetectionHit(allDetectionHits, detectionHit);

    //Set behavior based on hit
    FVector goal;

    if (playerFound)
    {
        goal = isPlayerPowerUp ? FindBestHidingLocation() : playerLocation;

        if (FVector::DistXY(GetPawn()->GetActorLocation(), goal) < 100)
        {
            playerFound = false;
        }
    }

    if (!playerFound)
    {
        goal = FindNearestPickupLocation();
    }

    // Update target location
    m_targetLocation = goal;

    // Update path to target
    m_pathToTarget = ComputePathToTarget(goal);

    DrawDebugCapsule(GetWorld(), detectionStartLocation + m_DetectionCapsuleHalfLength * selfPawn->GetActorForwardVector(), m_DetectionCapsuleHalfLength, m_DetectionCapsuleRadius, selfPawn->GetActorQuat() * selfPawn->GetActorUpVector().ToOrientationQuat(), FColor::Blue);
}

void ASDTAIController::ShowNavigationPath()
{
    DisplayNavigationPath(m_pathToTarget, true);
}

void ASDTAIController::DisplayNavigationPath(UNavigationPath* path, bool active)
{
    // Shows current navigation path using DrawDebugLine and DrawDebugSphere
    FColor color = active ? FColor(255, 0, 0) : FColor(128, 128, 128);
    FVector height = active ? FVector(0, 0, 50) : FVector(0, 0, 51);
    float radius = active ? 10.0f : 5.0f;

    if (path != NULL && path->PathPoints.Num() > 1)
    {
        // For each point in the path from the AI agent to the target...
        for (int i = 0; i < path->PathPoints.Num(); ++i)
        {
            // Draw a small sphere at the point
            DrawDebugSphere(GetWorld(), path->PathPoints[i] + height, radius, 12, color);
            
            // If this point is not the last point in the path...
            if (i < path->PathPoints.Num() - 1) {
                // Draw a line that connects this point to the next point in the path
                DrawDebugLine(GetWorld(), path->PathPoints[i] + height, path->PathPoints[i + 1] + height, color);
            }
        }
    }
}

FVector ASDTAIController::FindNearestPickupLocation()
{
    float shortestPathLength = 999999999999.9f;
    FVector shortestPathTargetLocation = FVector();

    // For each pickup on the map...
    TArray<AActor*> pickups;
    UGameplayStatics::GetAllActorsOfClass(GetWorld(), ASDTCollectible::StaticClass(), pickups);
    for (auto* pickupActor : pickups) {

        // Only consider pickups that are not on cooldown
        auto* pickup = Cast<ASDTCollectible>(pickupActor);
        if (pickup->IsOnCooldown()) {
            continue;
        }

        // Get the pickup's location and compute the AI agent's path to its location
        FVector pickupLocation = pickup->GetActorLocation();
        UNavigationPath* pathToPickup = ComputePathToTarget(pickupLocation);
        DisplayNavigationPath(pathToPickup, false);

        // If the path's length is the shortest we have seen yet, we save it
        float pathToPickupLength = pathToPickup->GetPathLength();
        if (pathToPickupLength < shortestPathLength) {
            shortestPathLength = pathToPickupLength;
            shortestPathTargetLocation = pickupLocation;
        }
    }

    // We return the location of the nearest pickup based on shortest path length
    return shortestPathTargetLocation;
}

FVector ASDTAIController::FindBestHidingLocation()
{
    float shortestPathLength = 999999999999.9f;
    FVector shortestPathTargetLocation = FVector();

    // For each hiding location on the map...
    TArray<AActor*> hidingPlaces;
    UGameplayStatics::GetAllActorsOfClass(GetWorld(), ASDTFleeLocation::StaticClass(), hidingPlaces);
    for (auto* hidingActor : hidingPlaces) {

        auto* hidingPlace = Cast<ASDTFleeLocation>(hidingActor);

        // Get the hiding place location and compute the AI agent's path to its location
        FVector hidingLocation = hidingPlace->GetActorLocation();
        UNavigationPath* pathToHiding = ComputePathToTarget(hidingLocation);

        // If the path's length is the shortest we have seen yet and the agent has a smaller euclidian distance to the spot than the player, we save it
        float pathToHidingLength = pathToHiding->GetPathLength();
        float playerDistanceToSpot = FVector::DistXY(playerLocation, hidingPlace->GetActorLocation());
        FVector agentLocation = GetPawn()->GetActorLocation();
        float agentDistanceToSpot = FVector::DistXY(agentLocation, hidingPlace->GetActorLocation());

        bool playerTooClose = playerDistanceToSpot < agentDistanceToSpot;

        if ((pathToHidingLength < shortestPathLength) && !playerTooClose) {
            shortestPathLength = pathToHidingLength;
            shortestPathTargetLocation = hidingLocation;
        }
    }

    // We return the location of the nearest hiding place based on shortest path length
    return shortestPathTargetLocation;
}

UNavigationPath* ASDTAIController::ComputePathToTarget(FVector targetLocation) {
    UNavigationSystemV1* navigationSystem = UNavigationSystemV1::GetCurrent(GetWorld());
    FVector myLocation = GetPawn()->GetActorLocation();
    return navigationSystem->FindPathToLocationSynchronously(GetWorld(), myLocation, targetLocation);
}

/*
* Name: findPlayer
* Description:
    Function that verify if a player is in the agent's detection sphere
    without being behind a wall and return that position and player's
    power up status if a player is found.
* Args:
    hit (FHitResult) : A collision hit result
    playerFound (bool&) : True if a player is found, else false
    isPlayerPowerUp (bool&) : True if a player is powered up, else false
    playerLocation (FVector&) : player's location if player is found, else null
* Return: None
*/
void ASDTAIController::findPlayer(FHitResult hit) {
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

void ASDTAIController::GetHightestPriorityDetectionHit(const TArray<FHitResult>& hits, FHitResult& outDetectionHit)
{
    for (const FHitResult& hit : hits)
    {
        if (UPrimitiveComponent* component = hit.GetComponent())
        {
            if (component->GetCollisionObjectType() == COLLISION_PLAYER)
            {
                //we can't get more important than the player
                outDetectionHit = hit;
                findPlayer(hit);
                return;
            }
            else if (playerFound) {
                // The AI character has to get to the last player location
                return;
            }
            else if (component->GetCollisionObjectType() == COLLISION_COLLECTIBLE)
            {
                outDetectionHit = hit;
            }
        }
    }
}

void ASDTAIController::AIStateInterrupted()
{
    StopMovement();
    m_ReachedTarget = true;
    isPlayerPowerUp = false;
    playerFound = false;
    m_MovementSpeed = m_WalkingSpeed;
}

void ASDTAIController::SetJumpDistance(float factor) {

    USkeletalMeshComponent* skeleton = (USkeletalMeshComponent*)GetPawn()->GetComponentByClass(USkeletalMeshComponent::StaticClass());
    if (skeleton) {
        if (!FloorHeight.IsValid())
        {
            FloorHeight = MakeShared<float>(skeleton->GetRelativeLocation().Z);
        }
        skeleton->SetRelativeLocation(FVector(0, 0, *FloorHeight.Get() + JumpCurve->GetFloatValue(factor) * JumpApexHeight));
    }
}

void ASDTAIController::FinishedJumpingInitialization()
{
    InAir = true;
}

void ASDTAIController::FinishedLanding()
{
    InAir = false;
}
