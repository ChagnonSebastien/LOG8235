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

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = AI, Meta = (
        ShortTooltip = "Temps de reaparition",
        ToolTip = "Determine le temps de reapparition d'un collectible"))
        float m_CollectCooldownDuration = 10.f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = AI, Meta = (
        ShortTooltip = "Se deplace",
        ToolTip = "Determine si un collectible peut se deplacer"))
        bool isMoveable = false;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = AI, Meta = (
        ShortTooltip = "Vitesse maximale",
        ToolTip = "Determine la vitesse maximale a laquelle un collectible peut se deplacer"))
        float MAX_SPEED = 500.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = AI, Meta = (
        ShortTooltip = "Acceleration maximale",
        ToolTip = "Determine l'acceleration maximale a laquelle un collectible peut se deplacer"))
        float MAX_ACCELERATION = 500.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = AI, Meta = (
        ShortTooltip = "Distance minimum d'un mur",
        ToolTip = "Determine la distance minimale a laquelle un collectible se deplaçant peut s'approcher d'un mur"))
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
