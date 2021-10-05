// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/StaticMeshActor.h"
#include "SDTCollectible.generated.h"

/**
 *
 */
UCLASS()
class SOFTDESIGNTRAINING_API ASDTCollectible : public AStaticMeshActor
{
    GENERATED_BODY()
public:
    ASDTCollectible();

    void Collect();
    void OnCooldownDone();
    bool IsOnCooldown();
    /*
    * Name: m_CollectCooldownDuration
    * Description: Time before the collectible respawn after being taken
    */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = AI)
        float m_CollectCooldownDuration = 10.f;
    /*
    * Name: isMoveable
    * Description: true if collectible can move, else false
    */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = AI)
        bool isMoveable = false;

    /*
    * Name: MAX_SPEED
    * Description: Maximum speed of the moveable collectible
    */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = AI)
        float MAX_SPEED = 500.0f;

    /*
    * Name: MAX_ACCELERATION
    * Description: Length of the acceleration vector
    */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = AI)
        float MAX_ACCELERATION = 500.0f;
    /*
    * Name: MIN_DIST_WALL
    * Description: Minimum distance between the wall and the moveable collectible
    */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = AI)
        float MIN_DIST_WALL = 200.f;
    
    virtual void Tick(float deltaTime) override;
    virtual void BeginPlay() override;

protected:
    void SetToInitialState();

    FTimerHandle m_CollectCooldownTimer;
    FVector m_initialPosition;
    FVector m_speed;
    FVector m_acceleration;
};
