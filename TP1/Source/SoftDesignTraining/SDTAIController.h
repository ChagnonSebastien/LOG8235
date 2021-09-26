// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"

#include "SDTAIController.generated.h"

const float SIGHT_THRESHOLD = 400.f;
const float ENVY_MAGNITUDE = 0.2f;
const float ROTATING_SPEED = 3.0f;
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

    virtual void Tick(float deltaTime) override;

private:
    FRotator envy = FRotator(0, 1, 0);

    // -1 rotating left, 1 rotating right, 0 not escaping;
    float escapingCorner = 0;

    virtual void computeNeasestCollision(float& distance, FVector_NetQuantizeNormal& hitNormal, TArray<struct FHitResult> hits);
};
