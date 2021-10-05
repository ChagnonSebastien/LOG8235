/*
* STDAIController.h
* Authors:
    - Sébastien Chagnon
    - Andy Lam
    - Jason Thai
    - Alexandre Ramtoula
    - Philippe Trempe
*/
#pragma once

#include "CoreMinimal.h"
#include "AIController.h"

#include "SDTAIController.generated.h"

/**
 * Class that defines an agent behaviour
 */
UCLASS(ClassGroup = AI, config = Game)
class SOFTDESIGNTRAINING_API ASDTAIController : public AAIController
{
    GENERATED_BODY()
public:
    /*
    * Name: debug
    * Description: Show debug logs and lines if set to True
    */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = AI)
        bool debug = false;
    /*
    * Name: sightThreshold
    * Description: Agent's detection sight threshhold
    */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = AI)
        float sightThreshold = 400.f;
    /*
    * Name: envyStrength
    * Description: Agent's envy strength
    */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = AI)
        float envyStrength = 0.2f;
    /*
    * Name: rotatingSpeed
    * Description: Agent's rotating speed
    */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = AI)
        float rotatingSpeed = 3.0f;
    /*
    * Name: detectionSphereRadius
    * Description: Agent's detection sphere radius
    */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = AI)
        float detectionSphereRadius = 200.f;

    virtual void Tick(float deltaTime) override;

private:
    FRotator envy = FRotator(0, 1, 0);

    // -1 rotating left, 1 rotating right, 0 not escaping;
    int escapingCorner = 0;

    virtual void computeNearestCollision(float& distance, FVector_NetQuantizeNormal& hitNormal, TArray<struct FHitResult> hits);
    virtual void freeRoam(float& speed, FRotator& walkingDirection, TArray<struct FHitResult> centerHitResults, TArray<struct FHitResult> rightHitResults, TArray<struct FHitResult> leftHitResults, float deltaTime);
    virtual void chaseObject(FRotator& walkingDirection, FVector objectLocation);
    virtual void findCollectible(FHitResult hit, bool& collectibleFound, FVector& collectibleLocation);
    virtual void findPlayer(FHitResult hit, bool& playerFound, FVector& playerLocation, bool& isPlayerPowerUp);
    virtual void sweepForwardObjects(bool& collectibleFound, FVector& collectibleLocation, bool& playerFound, bool& isPlayerPowerUp, FVector& playerLocation);
};
