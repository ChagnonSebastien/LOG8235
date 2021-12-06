/*
* BTTask_ExpectPlayerPoweredUp.cpp
* Authors:
    - Sebastien Chagnon
    - Andy Lam
    - Jason Thai
    - Alexandre Ramtoula
    - Philippe Trempe
*/

#include "BTTask_ExpectPlayerPoweredUp.h"
#include "SoftDesignTraining.h"
#include "SDTUtils.h"
#include "SoftDesignTrainingCharacter.h"

/**
* Update profiling scope, verify if the player is currently powered up, and lastly log execution time.
* @param OwnerComp The behavior Tree
* @return Returns node succeeded if the player is powered up, else node failed
*/
EBTNodeResult::Type UBTTask_ExpectPlayerPoweredUp::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
    ASDTAIController* aiController = Cast<ASDTAIController>(OwnerComp.GetAIOwner());
    aiController->m_profiler.startProfilingScope("DETECT");

    bool poweredUp = SDTUtils::IsPlayerPoweredUp(GetWorld());

    aiController->m_profiler.stopProfilingScope("DETECT");
    aiController->budget->LogExecutionTime(FString(TEXT("DETECT")), aiController->m_profiler.getScopeElapsedSeconds("DETECT"));

    return poweredUp ? EBTNodeResult::Succeeded : EBTNodeResult::Failed;
}

