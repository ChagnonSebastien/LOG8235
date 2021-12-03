// Fill out your copyright notice in the Description page of Project Settings.


#include "MyBTTask_Fleeing.h"

EBTNodeResult::Type UMyBTTask_Fleeing::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	if (ASDTAIController* aiController = Cast<ASDTAIController>(OwnerComp.GetAIOwner())) {

		aiController->MoveToBestFleeLocation();
		return EBTNodeResult::Succeeded;
	}
	return EBTNodeResult::Failed;
}