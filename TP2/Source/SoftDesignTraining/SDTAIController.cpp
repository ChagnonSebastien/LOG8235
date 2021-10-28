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
#include "SDTUtils.h"
#include "EngineUtils.h"
#include "SDTCollectible.h"

ASDTAIController::ASDTAIController(const FObjectInitializer& ObjectInitializer)
    : Super(ObjectInitializer.SetDefaultSubobjectClass<USDTPathFollowingComponent>(TEXT("PathFollowingComponent")))
{
}

void ASDTAIController::GoToBestTarget(float deltaTime)
{
    //Move to target depending on current behavior
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
        DisplayNavigationPath(pathToPickup, FColor(128, 128, 128));
        
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

FVector ASDTAIController::FindNearestHidingLocation()
{
    float shortestPathLength = 999999999999.9f;
    FVector shortestPathTargetLocation = FVector();

    // For each hiding location on the map...
    TArray<AActor*> hidingPlaces;
    UGameplayStatics::GetAllActorsOfClass(GetWorld(), ASDTFleeLocation::StaticClass(), hidingPlaces);
    for (auto* hidingActor : hidingPlaces) {

        // Only consider pickups that are not on cooldown
        auto* hidingPlace = Cast<ASDTFleeLocation>(hidingActor);

        // Get the hiding place location and compute the AI agent's path to its location
        FVector hidingLocation = hidingPlace->GetActorLocation();
        UNavigationPath* pathToHiding = ComputePathToTarget(hidingLocation);

        // If the path's length is the shortest we have seen yet, we save it
        float pathToHidingLength = pathToHiding->GetPathLength();
        if (pathToHidingLength < shortestPathLength) {
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

void ASDTAIController::UpdateTarget(FVector targetLocation)
{
    // Update target location
    m_targetLocation = targetLocation;

    // Update path to target
    m_pathToTarget = ComputePathToTarget(targetLocation);

    // Update path follower with new path
    GetPathFollowingComponent()->RequestMove(FAIMoveRequest(), m_pathToTarget->GetPath());
}

void ASDTAIController::ShowNavigationPath()
{
    DisplayNavigationPath(m_pathToTarget, FColor(255, 0, 0));
}

void ASDTAIController::DisplayNavigationPath(UNavigationPath* path, FColor color)
{
    // Shows current navigation path using DrawDebugLine and DrawDebugSphere

    if (path != NULL)
    {
        // For each point in the path from the AI agent to the target...
        for (int i = 0; i < path->PathPoints.Num(); ++i)
        {
            // Draw a small sphere at the point
            DrawDebugSphere(GetWorld(), path->PathPoints[i], 10.0f, 12, color);
            
            // If this point is not the last point in the path...
            if (i < path->PathPoints.Num() - 1) {
                // Draw a line that connects this point to the next point in the path
                DrawDebugLine(GetWorld(), path->PathPoints[i], path->PathPoints[i + 1], color);
            }
        }
    }
}

void ASDTAIController::ChooseBehavior(float deltaTime)
{
    UpdatePlayerInteraction(deltaTime);
}

void ASDTAIController::UpdatePlayerInteraction(float deltaTime)
{

    bool playerFound = false; // TO MOOVE ELSEWHERE
    bool isPlayerPowerUp = false;  // TO MOOVE ELSEWHERE
    //finish jump before updating AI state
    if (AtJumpSegment)
        return;

    APawn* selfPawn = GetPawn();
    if (!selfPawn)
        return;

    ACharacter* playerCharacter = UGameplayStatics::GetPlayerCharacter(GetWorld(), 0);
    if (!playerCharacter)
        return;

    FVector detectionStartLocation = selfPawn->GetActorLocation() + selfPawn->GetActorForwardVector() * m_DetectionCapsuleForwardStartingOffset;
    FVector detectionEndLocation = detectionStartLocation + selfPawn->GetActorForwardVector() * m_DetectionCapsuleHalfLength * 2;

    TArray<TEnumAsByte<EObjectTypeQuery>> detectionTraceObjectTypes;
    detectionTraceObjectTypes.Add(UEngineTypes::ConvertToObjectType(COLLISION_COLLECTIBLE));
    detectionTraceObjectTypes.Add(UEngineTypes::ConvertToObjectType(COLLISION_PLAYER));

    TArray<FHitResult> allDetectionHits;
    GetWorld()->SweepMultiByObjectType(allDetectionHits, detectionStartLocation, detectionEndLocation, FQuat::Identity, detectionTraceObjectTypes, FCollisionShape::MakeSphere(m_DetectionCapsuleRadius));

    FHitResult detectionHit;
    GetHightestPriorityDetectionHit(allDetectionHits, detectionHit, playerFound, isPlayerPowerUp);

    //Set behavior based on hit

    FVector nearestPickupLocation = FindNearestPickupLocation();
    

    if (playerFound && isPlayerPowerUp)
    {
        // Agent is escaping from player
        nearestPickupLocation = FindNearestHidingLocation();
    }

    UpdateTarget(nearestPickupLocation);

    DrawDebugCapsule(GetWorld(), detectionStartLocation + m_DetectionCapsuleHalfLength * selfPawn->GetActorForwardVector(), m_DetectionCapsuleHalfLength, m_DetectionCapsuleRadius, selfPawn->GetActorQuat() * selfPawn->GetActorUpVector().ToOrientationQuat(), FColor::Blue);
    ShowNavigationPath();
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
void ASDTAIController::findPlayer(FHitResult hit, bool& playerFound, bool& isPlayerPowerUp) {
    struct FHitResult hitResult;
    FCollisionQueryParams params = FCollisionQueryParams();
    params.AddIgnoredActor(GetPawn());
    if (GetWorld()->LineTraceSingleByObjectType(hitResult, GetPawn()->GetActorLocation(), hit.GetActor()->GetActorLocation(), FCollisionObjectQueryParams::AllObjects, params)) {
        if (hitResult.GetComponent()->GetCollisionObjectType() == COLLISION_PLAYER) {
            if (ASoftDesignTrainingMainCharacter* player = Cast<ASoftDesignTrainingMainCharacter>(hit.GetActor())) {
                playerFound = true;
                //playerLocation = hit.GetActor()->GetActorLocation();  TO USE?
                isPlayerPowerUp = player->IsPoweredUp();
            }
        }
    }
}

void ASDTAIController::GetHightestPriorityDetectionHit(const TArray<FHitResult>& hits, FHitResult& outDetectionHit, bool& playerFound, bool& isPlayerPowerUp)
{
    for (const FHitResult& hit : hits)
    {
        if (UPrimitiveComponent* component = hit.GetComponent())
        {
            if (component->GetCollisionObjectType() == COLLISION_PLAYER)
            {
                //we can't get more important than the player
                outDetectionHit = hit;
                findPlayer(hit, playerFound, isPlayerPowerUp);
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
}