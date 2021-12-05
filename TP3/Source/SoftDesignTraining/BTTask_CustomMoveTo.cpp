// Fill out your copyright notice in the Description page of Project Settings.


#include "BTTask_CustomMoveTo.h"


EBTNodeResult::Type UBTTask_CustomMoveTo::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
    if (ASDTAIController* aiController = Cast<ASDTAIController>(OwnerComp.GetAIOwner()))
    {
        FVector target = OwnerComp.GetBlackboardComponent()->GetValueAsVector(GetSelectedBlackboardKey());
        aiController->MoveToLocation(target, 0.5f, false, true, true, NULL, false);
        aiController->OnMoveToTarget();

        return EBTNodeResult::Succeeded;
    }

    return EBTNodeResult::Failed;
}