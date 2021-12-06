/*
* BTTask_CustomMoveTo.h
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
#include "BTTask_CustomMoveTo.generated.h"

/**
 * Behavior Tree Task CustomMoveTo class.
 */
UCLASS()
class SOFTDESIGNTRAINING_API UBTTask_CustomMoveTo : public UBTTask_BlackboardBase
{
	GENERATED_BODY()
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
};
