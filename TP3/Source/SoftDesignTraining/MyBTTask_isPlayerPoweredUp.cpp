// Fill out your copyright notice in the Description page of Project Settings.

#include "MyBTTask_isPlayerPoweredUp.h"
#include "SoftDesignTraining.h"
#include "SDTUtils.h"
#include "AiAgentGroupManager.h"
#include "SoftDesignTrainingCharacter.h"
#include "SDTAIController.h"

EBTNodeResult::Type UMyBTTask_isPlayerPoweredUp::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{

    if (SDTUtils::IsPlayerPoweredUp(GetWorld()))
    {
        ASDTAIController* aiController = Cast<ASDTAIController>(OwnerComp.GetAIOwner());
        AiAgentGroupManager* groupManager = AiAgentGroupManager::GetInstance();
        if (groupManager) {
            groupManager->UnregisterAll();
        }

        return EBTNodeResult::Succeeded;
    }

    return EBTNodeResult::Failed;
}