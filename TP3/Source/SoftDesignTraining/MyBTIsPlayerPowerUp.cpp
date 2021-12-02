// Fill out your copyright notice in the Description page of Project Settings.


#include "MyBTIsPlayerPowerUp.h"
#include "BehaviorTree/Blackboard/BlackboardKeyType_Bool.h"
#include "SoftDesignTraining.h"
#include "SDTUtils.h"
#include "SDTAIController.h"
#include "SoftDesignTrainingCharacter.h"

UMyBTIsPlayerPowerUp::UMyBTIsPlayerPowerUp()
{
    bCreateNodeInstance = true;
}

void UMyBTIsPlayerPowerUp::TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
    if (ASDTAIController* aiController = Cast<ASDTAIController>(OwnerComp.GetAIOwner()))
    {
        if (SDTUtils::IsPlayerPoweredUp(GetWorld()))
        {
            OwnerComp.GetBlackboardComponent()->SetValue<UBlackboardKeyType_Bool>(aiController->GetTargetPowerUpKeyID(), false);
        }
        else {
            OwnerComp.GetBlackboardComponent()->SetValue<UBlackboardKeyType_Bool>(aiController->GetTargetPowerUpKeyID(), true);
            aiController->m_profiler.stopProfilingScope("DETECT");
        }
    }
}