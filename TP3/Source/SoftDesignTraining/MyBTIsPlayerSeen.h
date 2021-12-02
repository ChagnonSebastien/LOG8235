// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTService.h"
#include "BehaviorTree/Blackboard/BlackboardKeyType_Vector.h"
#include "MyBTIsPlayerSeen.generated.h"


/**
 *
 */
UCLASS()
class SOFTDESIGNTRAINING_API UMyBTIsPlayerSeen : public UBTService
{
	GENERATED_BODY()

public:
	UMyBTIsPlayerSeen();

	virtual void TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;
};
