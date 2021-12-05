// Fill out your copyright notice in the Description page of Project Settings.


#include "BTTask_CustomMoveTo.h"


EBTNodeResult::Type UBTTask_CustomMoveTo::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
    ASDTAIController* aiController = Cast<ASDTAIController>(OwnerComp.GetAIOwner());

    if (aiController->AtJumpSegment)
    {
        // No repathing while jumping
        return EBTNodeResult::Succeeded;
    }

    FVector target = OwnerComp.GetBlackboardComponent()->GetValueAsVector(GetSelectedBlackboardKey());

    if (FVector::Dist2D(target, aiController->GetCurrentTargetPosition()) < 100)
    {
        // Not enough change to justify a repathing
        return EBTNodeResult::Succeeded;
    }

    aiController->MoveToLocation(target, 0.5f, false, true, true, NULL, false);
    aiController->OnMoveToTarget();

    return EBTNodeResult::Succeeded;
}