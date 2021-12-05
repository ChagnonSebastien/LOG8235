// Fill out your copyright notice in the Description page of Project Settings.
#include <iomanip>
#include <sstream>

#include "SDTAIController.h"
#include "SoftDesignTraining.h"
#include "SDTCollectible.h"
#include "SDTFleeLocation.h"
#include "SDTPathFollowingComponent.h"
#include "DrawDebugHelpers.h"
#include "BehaviorTree/Blackboard/BlackboardKeyType_Bool.h"
#include "BehaviorTree/Blackboard/BlackboardKeyType_Object.h"
#include "SoftDesignTrainingCharacter.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "AiAgentGroupManager.h"
#include "Kismet/KismetMathLibrary.h"
#include "SDTUtils.h"
#include "EngineUtils.h"

ASDTAIController::ASDTAIController(const FObjectInitializer& ObjectInitializer)
    : Super(ObjectInitializer.SetDefaultSubobjectClass<USDTPathFollowingComponent>(TEXT("PathFollowingComponent")))
{
    m_behaviorTreeComponent = CreateDefaultSubobject<UBehaviorTreeComponent>(TEXT("BehaviorTreeComponent"));
    m_blackboardComponent = CreateDefaultSubobject<UBlackboardComponent>(TEXT("BlackBoardComponent"));
}

void ASDTAIController::BeginPlay()
{
    Super::BeginPlay();
    id = rand();

    // Budget Allocation Setup
    TArray<AActor*> foundBudgets;
    UGameplayStatics::GetAllActorsOfClass(GetWorld(), BudgetClass, foundBudgets);
    budget = (ATimeBudget*)foundBudgets.Pop();
    budget->registerController(id);


    // Order: budget reset -> profiler_reset -> Behavior
    this->AddTickPrerequisiteActor(budget);
    m_behaviorTreeComponent->AddTickPrerequisiteActor(budget);
    m_blackboardComponent->AddTickPrerequisiteActor(budget);
    m_behaviorTreeComponent->AddTickPrerequisiteActor(this);
    m_blackboardComponent->AddTickPrerequisiteActor(this);
}

void ASDTAIController::Tick(float deltaTime)
{
    DisplayProfilerTimes(deltaTime);
    m_profiler.reset();
}

/// <summary>
/// Displays elapsed time for profiled scopes above the agent.
/// </summary>
void ASDTAIController::DisplayProfilerTimes(float deltaTime) {
    auto numScopes = m_profiler.scopes.size();
    for (int i = 0; i < numScopes; ++i) {
        std::string scope = m_profiler.scopes[i];
        std::stringstream stream;
        stream << scope << ": " << std::fixed << std::setprecision(3) << m_profiler.getScopeElapsedSeconds(scope) * 1000 << " ms";
        std::string text = stream.str();
        DrawDebugString(GetWorld(), FVector(0.0f, 0.0f, 100.0f * (numScopes - i)), text.c_str(), GetPawn(), FColor::White, deltaTime, false);
    }
}

void ASDTAIController::StartBehaviorTree(APawn* pawn)
{
    if (ASoftDesignTrainingCharacter* aiBaseCharacter = Cast<ASoftDesignTrainingCharacter>(pawn))
    {

        if (aiBaseCharacter->GetBehaviorTree())
        {
            m_behaviorTreeComponent->StartTree(*aiBaseCharacter->GetBehaviorTree());
        }
    }
}

void ASDTAIController::OnPossess(APawn* pawn)
{
    Super::OnPossess(pawn);

    if (ASoftDesignTrainingCharacter* aiBaseCharacter = Cast<ASoftDesignTrainingCharacter>(pawn))
    {
        if (aiBaseCharacter->GetBehaviorTree())
        {
            m_blackboardComponent->InitializeBlackboard(*aiBaseCharacter->GetBehaviorTree()->BlackboardAsset);
        }
    }
}

FVector ASDTAIController::GetCurrentTargetPosition()
{
    return GetPathFollowingComponent()->GetCurrentTargetLocation();
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

void ASDTAIController::ShowNavigationPath()
{
    if (UPathFollowingComponent* pathFollowingComponent = GetPathFollowingComponent())
    {
        if (pathFollowingComponent->HasValidPath())
        {
            const FNavPathSharedPtr path = pathFollowingComponent->GetPath();
            TArray<FNavPathPoint> pathPoints = path->GetPathPoints();

            for (int i = 0; i < pathPoints.Num(); ++i)
            {
                DrawDebugSphere(GetWorld(), pathPoints[i].Location, 10.f, 8, FColor::Yellow);

                if (i != 0)
                {
                    DrawDebugLine(GetWorld(), pathPoints[i].Location, pathPoints[i - 1].Location, FColor::Yellow);
                }
            }
        }
    }
}

bool ASDTAIController::IsPlayerSeen()
{
    //finish jump before updating AI state
    if (AtJumpSegment)
        return false;

    APawn* selfPawn = GetPawn();
    if (!selfPawn)
        return false;

    ACharacter* playerCharacter = UGameplayStatics::GetPlayerCharacter(GetWorld(), 0);
    if (!playerCharacter)
        return false;

    FVector detectionStartLocation = selfPawn->GetActorLocation() + selfPawn->GetActorForwardVector() * m_DetectionCapsuleForwardStartingOffset;
    FVector detectionEndLocation = detectionStartLocation + selfPawn->GetActorForwardVector() * m_DetectionCapsuleHalfLength * 2;

    TArray<TEnumAsByte<EObjectTypeQuery>> detectionTraceObjectTypes;
    detectionTraceObjectTypes.Add(UEngineTypes::ConvertToObjectType(COLLISION_PLAYER));

    TArray<FHitResult> allDetectionHits;
    GetWorld()->SweepMultiByObjectType(allDetectionHits, detectionStartLocation, detectionEndLocation, FQuat::Identity, detectionTraceObjectTypes, FCollisionShape::MakeSphere(m_DetectionCapsuleRadius));

    FHitResult detectionHit;
    GetHightestPriorityDetectionHit(allDetectionHits, detectionHit);

    bool isPlayerSeen = false;
    if (HasLoSOnHit(detectionHit)) {
        isPlayerSeen = true;
    }

    return isPlayerSeen;

}

bool ASDTAIController::HasLoSOnHit(const FHitResult& hit)
{
    if (!hit.GetComponent())
        return false;

    TArray<TEnumAsByte<EObjectTypeQuery>> TraceObjectTypes;
    TraceObjectTypes.Add(UEngineTypes::ConvertToObjectType(ECollisionChannel::ECC_WorldStatic));

    FVector hitDirection = hit.ImpactPoint - hit.TraceStart;
    hitDirection.Normalize();

    FHitResult losHit;
    FCollisionQueryParams queryParams = FCollisionQueryParams();
    queryParams.AddIgnoredActor(hit.GetActor());

    GetWorld()->LineTraceSingleByObjectType(losHit, hit.TraceStart, hit.ImpactPoint + hitDirection, TraceObjectTypes, queryParams);

    return losHit.GetActor() == nullptr;
}

void ASDTAIController::AIStateInterrupted()
{
    StopMovement();
    AtJumpSegment = false;
    Landing = true;
    m_ReachedTarget = true;
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
                return;
            }
            else if(component->GetCollisionObjectType() == COLLISION_COLLECTIBLE)
            {
                outDetectionHit = hit;
            }
        }
    }
}
