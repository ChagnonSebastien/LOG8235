// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameState.h"
#include "SoftDesignTrainingGameState.generated.h"

/**
 * 
 */
UCLASS()
class SOFTDESIGNTRAINING_API ASoftDesignTrainingGameState : public AGameState
{
	GENERATED_BODY()
public:


	void incrementDeaths();
	void incrementCollected();

	int getDeaths();
	int getCollected();

private:
	int numberOfDeaths = 0;
	int numberOfCollected = 0;
};

