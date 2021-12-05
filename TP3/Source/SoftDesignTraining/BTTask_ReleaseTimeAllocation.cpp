// Fill out your copyright notice in the Description page of Project Settings.


#include "BTTask_ReleaseTimeAllocation.h"


EBTNodeResult::Type UBTTask_ReleaseTimeAllocation::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
    if (ASDTAIController* aiController = Cast<ASDTAIController>(OwnerComp.GetAIOwner()))
    {
        aiController->m_profiler.stopProfilingScope("UPDATE");
    }

    return EBTNodeResult::Succeeded;
}