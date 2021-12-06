/*
* BTTask_ExpectPlayerPoweredUp.h
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
#include "AiAgentGroupManager.h"
#include "SDTAIController.h"
#include "BTTask_ExpectPlayerPoweredUp.generated.h"

/**
 * Behavior Tree Task ExpectPlayerPoweredUp class.
 */
UCLASS()
class SOFTDESIGNTRAINING_API UBTTask_ExpectPlayerPoweredUp : public UBTTask_BlackboardBase
{
	GENERATED_BODY()
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
};
