/*
* BTTask_Chasing.h
* Authors:
    - Sebastien Chagnon
    - Andy Lam
    - Jason Thai
    - Alexandre Ramtoula
    - Philippe Trempe
*/
#pragma once

#include "SDTAIController.h"
#include "SoftDesignTraining.h"
#include "SDTCollectible.h"
#include "SDTFleeLocation.h"
#include "SDTPathFollowingComponent.h"
#include "AiAgentGroupManager.h"
#include "DrawDebugHelpers.h"
#include "Kismet/KismetMathLibrary.h"
#include "BehaviorTree/Blackboard/BlackboardKeyType_Bool.h"
#include "SDTUtils.h"
#include "EngineUtils.h"
#include "CoreMinimal.h"
#include "BehaviorTree/Tasks/BTTask_BlackboardBase.h"
#include "BTTask_Chasing.generated.h"

/**
 * Behavior Tree Task Chasing class.
 */
UCLASS()
class SOFTDESIGNTRAINING_API UBTTask_Chasing : public UBTTask_BlackboardBase
{
	GENERATED_BODY()

	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;

};
