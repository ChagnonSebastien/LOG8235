// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/Tasks/BTTask_BlackboardBase.h"
#include "GameFramework/Pawn.h"
#include "BehaviorTree/BehaviorTreeComponent.h"
#include "BehaviorTree/BehaviorTreeTypes.h"
#include "UObject/UObjectGlobals.h"
#include "BTTask_FindClosestCollectible.generated.h"

/**
 * 
 */
UCLASS()
class SOFTDESIGNTRAINING_API UBTTask_FindClosestCollectible : public UBTTask_BlackboardBase
{
	GENERATED_BODY()
public:
	UBTTask_FindClosestCollectible(FObjectInitializer const& ObjectInitializer);
	EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& owner_comp, uint8* node_memory);
};
