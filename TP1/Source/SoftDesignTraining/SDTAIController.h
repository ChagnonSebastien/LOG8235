// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"

#include "SDTAIController.generated.h"

/**
 * 
 */
UCLASS(ClassGroup = AI, config = Game)
class SOFTDESIGNTRAINING_API ASDTAIController : public AAIController
{
    GENERATED_BODY()
public:

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = AI)
        bool debug = true;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = AI)
        float sightThreshold = 400.f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = AI)
        float envyStrength = 0.2f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = AI)
        float rotatingSpeed = 3.0f;

    virtual void Tick(float deltaTime) override;

private:
    FRotator envy = FRotator(0, 1, 0);

    // -1 rotating left, 1 rotating right, 0 not escaping;
    int escapingCorner = 0;

    virtual void computeNeasestCollision(float& distance, FVector_NetQuantizeNormal& hitNormal, TArray<struct FHitResult> hits);
    virtual void freeRoam(float& speed, FRotator& walkingDirection, FVector feetCenter, float deltaTime);
    virtual bool detectDeathTrap(float& speed, FRotator& walkingDirection);
};
