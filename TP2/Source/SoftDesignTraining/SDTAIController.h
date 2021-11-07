// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "NavigationSystem.h"
#include "Templates/SharedPointer.h"
#include "SDTBaseAIController.h"
#include "SDTPathFollowingComponent.h"
#include "SDTAIController.generated.h"

// Forward declaration to avoir circular dependencies
class USDTPathFollowingComponent;

/**
 * An AI controller that is used to manage the agent's state, behavior and interactions with the player.
 */
UCLASS(ClassGroup = AI, config = Game)
class SOFTDESIGNTRAINING_API ASDTAIController : public ASDTBaseAIController
{
	GENERATED_BODY()

public:
    ASDTAIController(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

    /** The speed at which the agent moves (in cm/s). */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = AI)
    float m_MovementSpeed = 250.f;

    /** The candidate next speed (in cm/s). */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = AI)
    float m_NewSpeed = 250.f;

    /** The speed at which the agent walks (in cm/s). */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = AI)
    float m_WalkingSpeed = 250.f;

    /** The speed at which the agent runs (in cm/s). */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = AI)
    float m_RunningSpeed = 600.f;

    /** The half-length of the detection capsule (in cm). */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = AI)
    float m_DetectionCapsuleHalfLength = 500.f;

    /** The radius of the detection capsule (in cm). */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = AI)
    float m_DetectionCapsuleRadius = 250.f;

    /** The forward starting offset of the detection capsule (in cm). */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = AI)
    float m_DetectionCapsuleForwardStartingOffset = 100.f;

    /** The agent's jump curve that describes its movement over death traps. */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = AI)
    UCurveFloat* JumpCurve;

    /** The height of the highest point in the agent's jump curve. */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = AI)
    float JumpApexHeight = 300.f;

    /** The agent's speed when jumping. */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = AI)
    float JumpSpeed = 1.f;

    /** If the agent is at a jump segment. */
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = AI)
    bool AtJumpSegment = false;

    /** If the agent is in the air (jumping). */
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = AI)
    bool InAir = false;

    /** If the agent is landing. */
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = AI)
    bool Landing = true;

public:
    bool CloseToJumpSegment = false;

    virtual void OnMoveCompleted(FAIRequestID RequestID, const FPathFollowingResult& Result) override;
    void AIStateInterrupted();

    UFUNCTION(BlueprintCallable, Category = AI)
    void FinishedJumpingInitialization();

    UFUNCTION(BlueprintCallable, Category = AI)
    void FinishedLanding();

    void SetJumpDistance(float factor);

    void Jump(FVector landingPoint);

protected:
    void OnMoveToTarget();
    void GetHightestPriorityDetectionHit(const TArray<FHitResult>& hits, FHitResult& outDetectionHit);
    void UpdatePlayerInteraction(float deltaTime);
    bool fleeing = false;
    bool isPlayerPowerUp = false;
    bool playerFound = false;
    FVector playerLocation = { 9999,9999,9999 };
    FVector m_targetLocation = FVector();
    UNavigationPath* m_pathToTarget = nullptr;

private:
    TSharedPtr<float> FloorHeight = nullptr;
    FVector LandingPoint;
    FVector JumpingPoint;

    virtual void GoToBestTarget(float deltaTime) override;
    virtual void ChooseBehavior(float deltaTime) override;
    virtual void ShowNavigationPath() override;
    virtual void DisplayNavigationPath(UNavigationPath* path, bool main);
    virtual FVector FindNearestPickupLocation();
    virtual FVector FindBestHidingLocation();
    virtual UNavigationPath* ComputePathToTarget(FVector targetLocation);
    virtual void findPlayer(FHitResult hit);
    virtual void UpdateSpeed();
};
