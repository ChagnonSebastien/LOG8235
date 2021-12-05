// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Profiling.h"
#include "CoreMinimal.h"
#include "TimeBudget.h"
#include "SDTBaseAIController.h"
#include "Templates/SharedPointer.h"
#include "GameFramework/Actor.h"
#include "SDTAIController.generated.h"


/**
 * 
 */
UCLASS(ClassGroup = AI, config = Game)
class SOFTDESIGNTRAINING_API ASDTAIController : public ASDTBaseAIController
{
	GENERATED_BODY()

public:
    ASDTAIController(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = AI)
    float m_DetectionCapsuleHalfLength = 500.f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = AI)
    float m_DetectionCapsuleRadius = 250.f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = AI)
    float m_DetectionCapsuleForwardStartingOffset = 100.f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = AI)
    UCurveFloat* JumpCurve;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = AI)
    float JumpApexHeight = 300.f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = AI)
    float JumpSpeed = 1.f;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = AI)
    bool AtJumpSegment = false;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = AI)
    bool InAir = false;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = AI)
    bool Landing = false;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = AI)
    TSubclassOf<ATimeBudget> BudgetClass;

    profiling::Profiler m_profiler;
    

protected:

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
    
    UBlackboardComponent* GetBlackBoardComponent() const { return m_blackboardComponent; }

    void DisplayProfilerTimes(float deltaTime);

    bool IsPlayerSeen();

    ATimeBudget* budget;
    int id;

protected:
    virtual void OnPossess(APawn* pawn) override;
private:
    virtual void ShowNavigationPath() override;


protected:
    FVector m_JumpTarget;
    FRotator m_ObstacleAvoidanceRotation;
    FTimerHandle m_PlayerInteractionNoLosTimer;

    UPROPERTY(transient)
    UBehaviorTreeComponent* m_behaviorTreeComponent;
    UPROPERTY(transient)
    UBlackboardComponent* m_blackboardComponent;

};
