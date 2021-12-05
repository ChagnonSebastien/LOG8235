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
            GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, TEXT("Player is powered up"));
            OwnerComp.GetBlackboardComponent()->SetValue<UBlackboardKeyType_Bool>(aiController->GetTargetPowerUpKeyID(), true);
        }
        else {
            OwnerComp.GetBlackboardComponent()->SetValue<UBlackboardKeyType_Bool>(aiController->GetTargetPowerUpKeyID(), false);
            aiController->m_profiler.stopProfilingScope("DETECT");
        }
    }
}