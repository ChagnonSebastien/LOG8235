/*
* BTTask_ReleaseTimeAllocation.h
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
#include "BTTask_ReleaseTimeAllocation.generated.h"

/**
 * Behavior Tree Task ReleaseTimeAllocation class.
 */
UCLASS()
class SOFTDESIGNTRAINING_API UBTTask_ReleaseTimeAllocation : public UBTTask_BlackboardBase
{
	GENERATED_BODY()
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
};
