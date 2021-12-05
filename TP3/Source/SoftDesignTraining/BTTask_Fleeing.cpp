// Fill out your copyright notice in the Description page of Project Settings.


#include "BTTask_Fleeing.h"

EBTNodeResult::Type UBTTask_Fleeing::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	ASDTAIController* aiController = Cast<ASDTAIController>(OwnerComp.GetAIOwner());
	aiController->m_profiler.startProfilingScope("FLEE");

	FVector escapePosition = aiController->FindBestFleeLocation();
	OwnerComp.GetBlackboardComponent()->SetValueAsVector(GetSelectedBlackboardKey(), escapePosition);

	aiController->m_profiler.stopProfilingScope("FLEE");
	aiController->budget->LogExecutionTime(FString(TEXT("FLEE")), aiController->m_profiler.getScopeElapsedSeconds("FLEE"));

	return EBTNodeResult::Succeeded;
}