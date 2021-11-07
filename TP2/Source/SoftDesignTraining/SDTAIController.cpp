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

/**
* Constructs an ASDTAIController from an FObjectInitializer.
*
* @param ObjectInitializer The initializer to use to construct the ASDTAIController.
*/
ASDTAIController::ASDTAIController(const FObjectInitializer& ObjectInitializer)
    : Super(ObjectInitializer.SetDefaultSubobjectClass<USDTPathFollowingComponent>(TEXT("PathFollowingComponent")))
{
}

/**
* Updates agent speed and path to target.
*
* @param deltaTime The tick delta time.
*/
void ASDTAIController::GoToBestTarget(float deltaTime)
{
    UpdateSpeed();

    // Update path follower with new path
    if (AtJumpSegment) {
        GEngine->AddOnScreenDebugMessage(-1, deltaTime, FColor::Yellow, TEXT("AtJumpSegment"));
        FVector currentLocation = GetPawn()->GetActorLocation();
        FVector orientation = LandingPoint - currentLocation;

        // Check if we should initiate the landing sequence
        if (FVector2D(orientation).Size() < 75)
        {
            Landing = true;

            // Check if the goal position can be projected onto the NavMesh
            FNavLocation navLocation;
            bool isTargetPositionNavigable = Cast<UNavigationSystemV1>(GetWorld()->GetNavigationSystem())->ProjectPointToNavigation(currentLocation, navLocation);
            if (isTargetPositionNavigable)
            {
                m_pathToTarget = nullptr;
                AtJumpSegment = false;
                GetPawn()->SetActorLocation(FVector(LandingPoint.X, LandingPoint.Y, currentLocation.Z));
                SetJumpDistance(1);
                return;
            }

        }

        orientation.Normalize();

        // Smoothly update AI agent rotation based on orientation
        FQuat rotation = FRotationMatrix::MakeFromXZ(orientation, FVector::UpVector).ToQuat();
        GetPawn()->SetActorRotation(FMath::Lerp(GetPawn()->GetActorRotation(), orientation.Rotation(), 0.05f));

        // Update AI agent location based on orientation and movement speed
        FVector newLocation = currentLocation + m_MovementSpeed * deltaTime * orientation;
        GetPawn()->SetActorLocation(newLocation);

        float totalDistance = FVector2D::Distance(FVector2D(JumpingPoint), FVector2D(LandingPoint));
        float currentDistance = FVector2D::Distance(FVector2D(JumpingPoint), FVector2D(currentLocation));
        SetJumpDistance(currentDistance / totalDistance);

    }
    else if (m_pathToTarget != NULL) {
        GetPathFollowingComponent()->RequestMove(FAIMoveRequest(), m_pathToTarget->GetPath());
    }
    

}

/**
* Updates agent speed.
*/
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

/**
* Callback that gets called when the agent starts moving towards its target.
*/
void ASDTAIController::OnMoveToTarget()
{
    m_ReachedTarget = false;
}

/**
* Callback that gets called when the agent has reached its target.
*
* @param RequestID The request identifier.
* @param Result    The result from the path follower with metadata.
*/
void ASDTAIController::OnMoveCompleted(FAIRequestID RequestID, const FPathFollowingResult& Result)
{
    Super::OnMoveCompleted(RequestID, Result);

    m_ReachedTarget = true;
}

/**
* Makes the agent choose its behavior.
*
* @param deltaTime The tick delta time.
*/
void ASDTAIController::ChooseBehavior(float deltaTime)
{
    UpdatePlayerInteraction(deltaTime);

    ShowNavigationPath();
}

/**
* Updates the agent's interaction with the player.
*
* @param deltaTime The tick delta time.
*/
void ASDTAIController::UpdatePlayerInteraction(float deltaTime)
{
    //finish jump before updating AI state
    if (AtJumpSegment || CloseToJumpSegment)
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

        if (FVector::DistXY(GetPawn()->GetActorLocation(), goal) < 75)
        {
            //m_MovementSpeed = 0.f;
            playerFound = false;
        }
    }

    if (!playerFound)
    {
        goal = FindNearestPickupLocation();
    }

    // Check if the goal position can be projected onto the NavMesh
    FNavLocation navLocation;
    bool isTargetPositionNavigable = Cast<UNavigationSystemV1>(GetWorld()->GetNavigationSystem())->ProjectPointToNavigation(goal, navLocation);
    if (!isTargetPositionNavigable)
        return;

    // Update target location
    m_targetLocation = goal;

    // Update path to target
    m_pathToTarget = ComputePathToTarget(goal);
    
    DrawDebugCapsule(GetWorld(), detectionStartLocation + m_DetectionCapsuleHalfLength * selfPawn->GetActorForwardVector(), m_DetectionCapsuleHalfLength, m_DetectionCapsuleRadius, selfPawn->GetActorQuat() * selfPawn->GetActorUpVector().ToOrientationQuat(), FColor::Blue);
}

/**
* Shows the agent's current navigation path.
*/
void ASDTAIController::ShowNavigationPath()
{
    DisplayNavigationPath(m_pathToTarget, true);
}

/**
* Shows a navigation path using draw debug spheres and lines.
*
* @param path   The path to show.
* @param active If this path is the agent's current active path (vs other candidate paths).
*/
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

/**
* Finds the nearest pickup's location by determining the path (on the navmesh) to each pickup
* and selecting the shortest one.
* 
* @return The location of the nearest pickup.
*/
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

/**
* Function that chooses which hiding spot is the best hiding spot 
* for a given agent location and depending on the the player location.
* The best hiding spot is the closest spot to the agent where the player 
* is farther than the agent using the Euclidian distance as the heuristic.
*
* @param path   The path to show.
* @param active If this path is the agent's current active path (vs other candidate paths).
* 
* @return Chosen best hiding spot location.
*/
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

        
        float pathToHidingLength = pathToHiding->GetPathLength();
        float playerDistanceToSpot = FVector::DistXY(playerLocation, hidingPlace->GetActorLocation());
        FVector agentLocation = GetPawn()->GetActorLocation();
        float agentDistanceToSpot = FVector::DistXY(agentLocation, hidingPlace->GetActorLocation());

        bool playerTooClose = playerDistanceToSpot < agentDistanceToSpot;

        // If the path's length is the shortest we have seen yet and the agent has a smaller Euclidian distance to the spot than the player, we save it
        if ((pathToHidingLength < shortestPathLength) && !playerTooClose) {
            shortestPathLength = pathToHidingLength;
            shortestPathTargetLocation = hidingLocation;
        }
    }

    // We return the location of the nearest hiding place based on shortest path length
    return shortestPathTargetLocation;
}

/**
* Computes the path (on the navmesh) from the agent's location to a given target location.
*
* @param targetLocation The target location where the path should end.
* 
* @return The path (on the nasmesh) to the given target.
*/
UNavigationPath* ASDTAIController::ComputePathToTarget(FVector targetLocation) {
    UNavigationSystemV1* navigationSystem = UNavigationSystemV1::GetCurrent(GetWorld());
    FVector myLocation = GetPawn()->GetActorLocation();
    return navigationSystem->FindPathToLocationSynchronously(GetWorld(), myLocation, targetLocation);
}

/**
* Function that verify if a player is in the agent's detection sphere
* without being behind a wall and return that position and player's
* power up status if a player is found.
*
* @param hit A collision hit result.
*/
void ASDTAIController::findPlayer(FHitResult hit) {
    struct FHitResult hitResult;
    FCollisionQueryParams params = FCollisionQueryParams();
    params.AddIgnoredActor(GetPawn());

    UCapsuleComponent* const boundingBox = (UCapsuleComponent*)GetPawn()->GetComponentByClass(UCapsuleComponent::StaticClass());
    float boudingBoxRadius = boundingBox->GetScaledCapsuleRadius();
    FVector boundingBoxOffset = GetPawn()->GetActorForwardVector().GetSafeNormal();
    boundingBoxOffset.X = boundingBoxOffset.X * boudingBoxRadius;
    boundingBoxOffset.Y = boundingBoxOffset.Y * boudingBoxRadius;

    if (GetWorld()->LineTraceSingleByObjectType(hitResult, GetPawn()->GetActorLocation() + boundingBoxOffset, hit.GetActor()->GetActorLocation(), FCollisionObjectQueryParams::AllObjects, params)) {
        if (hitResult.GetComponent()->GetCollisionObjectType() == COLLISION_PLAYER) {
            if (ASoftDesignTrainingMainCharacter* player = Cast<ASoftDesignTrainingMainCharacter>(hit.GetActor())) {
                playerFound = true;
                playerLocation = hit.GetActor()->GetActorLocation();
                isPlayerPowerUp = player->IsPoweredUp();
            }
        }
    }
}

/**
* Determines the raycast detection hit with highest priority.
*
* @param[in]  hits            The hits from the raycast.
* @param[out] outDetectionHit The detection hit with highest priority.
*/
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

/**
* Sets the agent's state to interrupted.
*/
void ASDTAIController::AIStateInterrupted()
{
    StopMovement();
    m_ReachedTarget = true;
    isPlayerPowerUp = false;
    playerFound = false;
    m_MovementSpeed = 0;
}

/**
* Sets the agent's jump distance from the given factor.
*
* @param factor The factor that is passed to the jump curve.
*/
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

void ASDTAIController::Jump(FVector landingPoint)
{
    AtJumpSegment = true;
    Landing = false;
    LandingPoint = landingPoint;
    JumpingPoint = GetPawn()->GetActorLocation();
    CloseToJumpSegment = false;
}

/**
* Updates the agent's internal state to be in air (jump started).
*/
void ASDTAIController::FinishedJumpingInitialization()
{
    InAir = true;
}

/**
* Updates the agent's internal state to be gounded (jump ended).
*/
void ASDTAIController::FinishedLanding()
{
    InAir = false;
}
