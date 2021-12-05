// Fill out your copyright notice in the Description page of Project Settings.


#include "BTTask_RequestAllocation.h"

EBTNodeResult::Type UBTTask_RequestAllocation::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
    ASDTAIController* aiController = Cast<ASDTAIController>(OwnerComp.GetAIOwner());
    if (aiController->budget->requestAllocation(aiController->id))
    {
        return EBTNodeResult::Succeeded;
    }

    return EBTNodeResult::Failed;
}
