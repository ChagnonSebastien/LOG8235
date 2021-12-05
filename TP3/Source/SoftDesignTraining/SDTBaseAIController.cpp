// Fill out your copyright notice in the Description page of Project Settings.

#include "SDTBaseAIController.h"
#include "SoftDesignTrainingCharacter.h"
#include "SoftDesignTraining.h"

ASDTBaseAIController::ASDTBaseAIController(const FObjectInitializer& ObjectInitializer)
    :Super(ObjectInitializer)
{
    PrimaryActorTick.bCanEverTick = true;
    PrimaryActorTick.bStartWithTickEnabled = true;
    m_ReachedTarget = true;
}

void ASDTBaseAIController::Tick(float deltaTime)
{
    Super::Tick(deltaTime);
    ShowNavigationPath();
}


