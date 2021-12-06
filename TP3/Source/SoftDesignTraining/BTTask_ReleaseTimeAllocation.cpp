/*
* BTTask_ReleaseTimeAllocation.cpp
* Authors:
    - Sebastien Chagnon
    - Andy Lam
    - Jason Thai
    - Alexandre Ramtoula
    - Philippe Trempe
*/

#include "BTTask_ReleaseTimeAllocation.h"

/**
* Stops profiling scope.
* @param OwnerComp The behavior Tree
* @return Returns node succeeded.
*/
EBTNodeResult::Type UBTTask_ReleaseTimeAllocation::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
    if (ASDTAIController* aiController = Cast<ASDTAIController>(OwnerComp.GetAIOwner()))
    {
        aiController->m_profiler.stopProfilingScope("UPDATE");
    }

    return EBTNodeResult::Succeeded;
}