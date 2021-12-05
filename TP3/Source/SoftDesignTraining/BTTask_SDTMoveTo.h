// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/Tasks/BTTask_BlackboardBase.h"
#include "BTTask_SDTMoveTo.generated.h"

/**
 * 
 */
UCLASS()
class SOFTDESIGNTRAINING_API UBTTask_SDTMoveTo : public UBTTask_BlackboardBase
{
	GENERATED_BODY()
public:
	UBTTask_SDTMoveTo(FObjectInitializer const& ObjectInitializer);
	EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& owner_comp, uint8* node_memory);
	
};
