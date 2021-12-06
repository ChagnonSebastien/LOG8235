/*
* SDTAIController.h
* Authors:
    - Sebastien Chagnon
    - Andy Lam
    - Jason Thai
    - Alexandre Ramtoula
    - Philippe Trempe
*/
#pragma once

#include "Profiling.h"
#include "CoreMinimal.h"
#include "TimeBudget.h"
#include "SDTBaseAIController.h"
#include "Templates/SharedPointer.h"
#include "GameFramework/Actor.h"
#include "SDTAIController.generated.h"


/**
 * AI agent controller class.
 */
UCLASS(ClassGroup = AI, config = Game)
class SOFTDESIGNTRAINING_API ASDTAIController : public ASDTBaseAIController
{
	GENERATED_BODY()

public:
    ASDTAIController(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

    /// <summary>
    /// Half of the length of the agent's detection capsule.
    /// </summary>
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = AI)
    float m_DetectionCapsuleHalfLength = 500.f;

    /// <summary>
    /// Radius of the agent's detection capsule.
    /// </summary>
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = AI)
    float m_DetectionCapsuleRadius = 250.f;

    /// <summary>
    /// Forward starting offset of the agent's detection capsule.
    /// </summary>
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = AI)
    float m_DetectionCapsuleForwardStartingOffset = 100.f;

    /// <summary>
    /// The agent's jump curve when jumping above death traps.
    /// </summary>
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = AI)
    UCurveFloat* JumpCurve;

    /// <summary>
    /// The maximal height reached by the agent when going througn the jump curve.
    /// </summary>
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = AI)
    float JumpApexHeight = 300.f;

    /// <summary>
    /// The agent's speed while jumping.
    /// </summary>
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = AI)
    float JumpSpeed = 1.f;

    /// <summary>
    /// Flag that determines if the agent is at a jump segment.
    /// </summary>
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = AI)
    bool AtJumpSegment = false;

    /// <summary>
    /// Flag that determines if the agent is in the air (is jumping).
    /// </summary>
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = AI)
    bool InAir = false;

    /// <summary>
    /// Flag that determines if the agent is landing (at the end of a jump).
    /// </summary>
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = AI)
    bool Landing = false;

    /// <summary>
    /// Agent's time budget class.
    /// </summary>
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = AI)
    TSubclassOf<ATimeBudget> BudgetClass;

    /// <summary>
    /// Agent's behavior tree component.
    /// </summary>
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = AI)
    UBehaviorTreeComponent* m_behaviorTreeComponent;

    /// <summary>
    /// Agent's blackboard component.
    /// </summary>
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = AI)
    UBlackboardComponent* m_blackboardComponent;

    /// <summary>
    /// Agent's ID.
    /// </summary>
    int id;

    /// <summary>
    /// Agent's profiler that chronometers scopes.
    /// </summary>
    profiling::Profiler m_profiler;

    /// <summary>
    /// Agent's time budget.
    /// </summary>
    ATimeBudget* budget;

protected:
    virtual void OnPossess(APawn* pawn) override;

    void GetHightestPriorityDetectionHit(const TArray<FHitResult>& hits, FHitResult& outDetectionHit);
    bool HasLoSOnHit(const FHitResult& hit);

public:
    void OnMoveToTarget();

    virtual void BeginPlay() override;
    virtual void Tick(float deltaTime) override;
    virtual void OnMoveCompleted(FAIRequestID RequestID, const FPathFollowingResult& Result) override;

    void AIStateInterrupted();
    void StartBehaviorTree(APawn* pawn);
    FVector GetCurrentTargetPosition();
    void DisplayProfilerTimes(float deltaTime);
    bool IsPlayerSeen();


private:
    virtual void ShowNavigationPath() override;

};
