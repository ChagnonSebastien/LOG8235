// Fill out your copyright notice in the Description page of Project Settings.

#include "SDTCollectible.h"
#include "SoftDesignTraining.h"

/**
* Constructs an ASDTCollectible.
*/
ASDTCollectible::ASDTCollectible() { }

/**
* A callback that is trigerred when this collectible is collected.
* This is where this collectible starts its cooldown period.
*/
void ASDTCollectible::Collect()
{
    GetWorld()->GetTimerManager().SetTimer(m_CollectCooldownTimer, this, &ASDTCollectible::OnCooldownDone, m_CollectCooldownDuration, false);

    GetStaticMeshComponent()->SetVisibility(false);
}

/**
* A callback that is trigerred when this colledtible's cooldown period is over.
* This is where this collectible's respawn occurs.
*/
void ASDTCollectible::OnCooldownDone()
{
    GetWorld()->GetTimerManager().ClearTimer(m_CollectCooldownTimer);

    GetStaticMeshComponent()->SetVisibility(true);
}

/**
* Determines is this collectible is currently on cooldown.
* 
* @return `true` if this collectible is on cooldown, `false` otherwise.
*/
bool ASDTCollectible::IsOnCooldown()
{
    return m_CollectCooldownTimer.IsValid();
}
