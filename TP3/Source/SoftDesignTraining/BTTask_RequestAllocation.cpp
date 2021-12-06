/*
* BTTask_RequestAllocation.cpp
* Authors:
    - Sebastien Chagnon
    - Andy Lam
    - Jason Thai
    - Alexandre Ramtoula
    - Philippe Trempe
*/

#include "BTTask_RequestAllocation.h"
/**
* Request budget allocation.
* @param OwnerComp The behavior Tree
* @return Returns Succeeded if the AI could request budget allocation, else False.
*/
EBTNodeResult::Type UBTTask_RequestAllocation::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
    ASDTAIController* aiController = Cast<ASDTAIController>(OwnerComp.GetAIOwner());
    if (aiController->budget->requestAllocation(aiController->id))
    {
        return EBTNodeResult::Succeeded;
    }

    return EBTNodeResult::Failed;
}
