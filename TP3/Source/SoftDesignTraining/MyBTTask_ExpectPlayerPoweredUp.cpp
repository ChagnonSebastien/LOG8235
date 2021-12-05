// Fill out your copyright notice in the Description page of Project Settings.


#include "MyBTTask_ExpectPlayerPoweredUp.h"
#include "SoftDesignTraining.h"
#include "SDTUtils.h"
#include "SoftDesignTrainingCharacter.h"

EBTNodeResult::Type UMyBTTask_ExpectPlayerPoweredUp::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
    ASDTAIController* aiController = Cast<ASDTAIController>(OwnerComp.GetAIOwner());
    aiController->m_profiler.startProfilingScope("DETECT");

    bool poweredUp = SDTUtils::IsPlayerPoweredUp(GetWorld());

    aiController->m_profiler.stopProfilingScope("DETECT");
    aiController->budget->LogExecutionTime(FString(TEXT("DETECT")), aiController->m_profiler.getScopeElapsedSeconds("DETECT"));

    return poweredUp ? EBTNodeResult::Succeeded : EBTNodeResult::Failed;
}

