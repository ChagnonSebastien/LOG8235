// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTService.h"
#include "MyBTIsPlayerPowerUp.generated.h"

/**
 * 
 */
UCLASS()
class SOFTDESIGNTRAINING_API UMyBTIsPlayerPowerUp : public UBTService
{
	GENERATED_BODY()

public:
	UMyBTIsPlayerPowerUp();

	virtual void TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;
};
