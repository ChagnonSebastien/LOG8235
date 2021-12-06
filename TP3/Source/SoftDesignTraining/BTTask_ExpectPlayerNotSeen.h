/*
* BTTask_ExpectPlayerNotSeen.h
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
#include "BehaviorTree/Blackboard/BlackboardKeyType_Vector.h"
#include "SDTAIController.h"
#include "BTTask_ExpectPlayerNotSeen.generated.h"

/**
 * Behavior Tree Task ExpectPlayerNotSeen class.
 */
UCLASS()
class SOFTDESIGNTRAINING_API UBTTask_ExpectPlayerNotSeen : public UBTTask_BlackboardBase
{
	GENERATED_BODY()
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
};
