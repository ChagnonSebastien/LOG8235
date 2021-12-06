/*
* BTTask_RequestAllocation.h
* Authors:
    - Sebastien Chagnon
    - Andy Lam
    - Jason Thai
    - Alexandre Ramtoula
    - Philippe Trempe
*/

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/Tasks/BTTask_BlackboardBase.h"
#include "SDTAIController.h"
#include "BTTask_RequestAllocation.generated.h"

/**
 * Behavior Tree Task RequestAllocation class.
 */
UCLASS()
class SOFTDESIGNTRAINING_API UBTTask_RequestAllocation : public UBTTaskNode
{
	GENERATED_BODY()
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
};
