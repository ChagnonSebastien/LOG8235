// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/Tasks/BTTask_BlackboardBase.h"
#include "BehaviorTree/Blackboard/BlackboardKeyType_Vector.h"
#include "MyBTTask_ExpectPlayerNotSeen.generated.h"

/**
 * 
 */
UCLASS()
class SOFTDESIGNTRAINING_API UMyBTTask_ExpectPlayerNotSeen : public UBTTask_BlackboardBase
{
	GENERATED_BODY()
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
};
