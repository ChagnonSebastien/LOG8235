/*
* BTTask_CustomMoveTo.cpp
* Authors:
    - Sebastien Chagnon
    - Andy Lam
    - Jason Thai
    - Alexandre Ramtoula
    - Philippe Trempe
*/
#include "BTTask_CustomMoveTo.h"

/**
* Update profiling scope, move AI controller to the target location, and lastly log execution time.
* @param OwnerComp The behavior Tree
* @return Returns node succeeded.
*/
EBTNodeResult::Type UBTTask_CustomMoveTo::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
    ASDTAIController* aiController = Cast<ASDTAIController>(OwnerComp.GetAIOwner());
    aiController->m_profiler.startProfilingScope("UPDATE");

    // No repathing while jumping
    if (!aiController->AtJumpSegment)
    {
        FVector target = OwnerComp.GetBlackboardComponent()->GetValueAsVector(GetSelectedBlackboardKey());

        // Verify if not enough change to justify a repathing
        float target_move_amount = FVector::Dist2D(target, aiController->GetCurrentTargetPosition());
        if (target_move_amount > 25.0f)
        {
            aiController->MoveToLocation(target, 0.5f, false, true, true, NULL, false);
            aiController->OnMoveToTarget();
        }
    }

    aiController->m_profiler.stopProfilingScope("UPDATE");
    aiController->budget->LogExecutionTime(FString(TEXT("UPDATE")), aiController->m_profiler.getScopeElapsedSeconds("UPDATE"));
    
    return EBTNodeResult::Succeeded;
}