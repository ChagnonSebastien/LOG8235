// Fill out your copyright notice in the Description page of Project Settings.


#include "BTTask_RequestAllocation.h"

EBTNodeResult::Type UBTTask_RequestAllocation::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
    if (ASDTAIController* aiController = Cast<ASDTAIController>(OwnerComp.GetAIOwner()))
    {
        if (aiController->budget->requestAllocation(aiController->id))
        {
            aiController->m_profiler.reset();
            aiController->m_profiler.startProfilingScope("UPDATE");
            return EBTNodeResult::Succeeded;
        }
    }

    return EBTNodeResult::Failed;
}
