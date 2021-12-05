// Fill out your copyright notice in the Description page of Project Settings.


#include "MyBTTask_Fleeing.h"

EBTNodeResult::Type UMyBTTask_Fleeing::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	if (ASDTAIController* aiController = Cast<ASDTAIController>(OwnerComp.GetAIOwner())) {

		FVector escapePosition = aiController->FindBestFleeLocation();
		OwnerComp.GetBlackboardComponent()->SetValueAsVector(GetSelectedBlackboardKey(), escapePosition);
		return EBTNodeResult::Succeeded;
	}
	return EBTNodeResult::Failed;
}