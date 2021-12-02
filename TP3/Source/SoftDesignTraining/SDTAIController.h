// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Profiling.h"
#include "CoreMinimal.h"
#include "SDTBaseAIController.h"
#include "TargetLKPInfo.h"
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

    

protected:

    enum PlayerInteractionBehavior
    {
        PlayerInteractionBehavior_Collect,
        PlayerInteractionBehavior_Chase,
        PlayerInteractionBehavior_Flee
    };

    void GetHightestPriorityDetectionHit(const TArray<FHitResult>& hits, FHitResult& outDetectionHit);
    void UpdatePlayerInteractionBehavior(const FHitResult& detectionHit, float deltaTime);
    PlayerInteractionBehavior GetCurrentPlayerInteractionBehavior(const FHitResult& hit);
    bool HasLoSOnHit(const FHitResult& hit);
    void MoveToRandomCollectible();
    void MoveToPlayer();
    void MoveToBestFleeLocation();
    void PlayerInteractionLoSUpdate();
    void OnPlayerInteractionNoLosDone();
    void OnMoveToTarget();

public:
    virtual void OnMoveCompleted(FAIRequestID RequestID, const FPathFollowingResult& Result) override;
    void RotateTowards(const FVector& targetLocation);
    void SetActorLocation(const FVector& targetLocation);
    void AIStateInterrupted();
    void StartBehaviorTree(APawn* pawn);
    void StopBehaviorTree(APawn* pawn);
    UBlackboardComponent* GetBlackBoardComponent() const { return m_blackboardComponent; }
    uint8 GetTargetPowerUpKeyID() const { return m_isTargetPowerUpKeyID; }
    uint8 GetTargetSeenKeyID() const { return m_isTargetSeenKeyID; }
    uint8 GetTargetPosBBKeyID() const { return m_targetPosBBKeyID; }
    TargetLKPInfo GetCurrentTargetLKPInfo() const { return m_targetLkpInfo;}
    void InvalidateTargetLKPInfo() { m_targetLkpInfo.SetLKPState(TargetLKPInfo::ELKPState::LKPState_Invalid); }
    void MoveToAssignedPos();

    uint8 GetTargetFleeLocationKeyID() const { return m_targetFleeLocationBBKeyID; }
    uint8 GetTargetPlayerLocationKeyID() const { return m_targetPlayerLocationBBKeyID; }

    PlayerInteractionBehavior GetPlayerInteractionBehavior() const { return m_PlayerInteractionBehavior; }
    bool IsPlayerSeen();


protected:
    virtual void OnPossess(APawn* pawn) override;
private:
    virtual void GoToBestTarget(float deltaTime) override;
    virtual void UpdatePlayerInteraction(float deltaTime) override;
    virtual void ShowNavigationPath() override;
    void DisplayProfilerTimes();
    



protected:
    FVector m_JumpTarget;
    FRotator m_ObstacleAvoidanceRotation;
    FTimerHandle m_PlayerInteractionNoLosTimer;
    PlayerInteractionBehavior m_PlayerInteractionBehavior;
    profiling::Profiler m_profiler;

    //
    UPROPERTY(transient)
        UBehaviorTreeComponent* m_behaviorTreeComponent;
    UPROPERTY(transient)
        UBlackboardComponent* m_blackboardComponent;
    uint8 m_isTargetPowerUpKeyID;
    uint8 m_isTargetSeenKeyID;
    uint8 m_targetPosBBKeyID;

    TargetLKPInfo m_targetLkpInfo;
 
    uint8 m_targetFleeLocationBBKeyID;
    uint8 m_targetPlayerLocationBBKeyID;
};
