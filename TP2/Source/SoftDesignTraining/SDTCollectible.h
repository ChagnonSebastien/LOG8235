// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/StaticMeshActor.h"
#include "SDTCollectible.generated.h"

/**
 * A collectible that the player can consume to gain a power-up, 
 * and that AI agents can consume to deny them to the player. 
 * When collected, the collectible becomes unavailable for the 
 * remainder of its cooldown duration.
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

    /** The cooldown duration (in seconds) of this collectible. */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = AI)
    float m_CollectCooldownDuration = 10.f;

protected:
    FTimerHandle m_CollectCooldownTimer;
	
};
