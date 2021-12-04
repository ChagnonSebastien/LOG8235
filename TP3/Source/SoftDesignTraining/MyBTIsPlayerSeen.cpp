// Fill out your copyright notice in the Description page of Project Settings.


#include "MyBTIsPlayerSeen.h"
#include "BehaviorTree/Blackboard/BlackboardKeyType_Bool.h"
#include "SoftDesignTraining.h"
#include "SDTUtils.h"
#include "SDTAIController.h"
#include "AiAgentGroupManager.h"
#include "SoftDesignTrainingCharacter.h"

UMyBTIsPlayerSeen::UMyBTIsPlayerSeen()
{
	bCreateNodeInstance = true;
}

void UMyBTIsPlayerSeen::TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
    if (ASDTAIController* aiController = Cast<ASDTAIController>(OwnerComp.GetAIOwner()))
    {
        aiController->m_profiler.reset();
        aiController->m_profiler.startProfilingScope("DETECT");
        ACharacter* playerCharacter = UGameplayStatics::GetPlayerCharacter(GetWorld(), 0);
        if (!playerCharacter) {
            OwnerComp.GetBlackboardComponent()->SetValue<UBlackboardKeyType_Bool>(aiController->GetTargetSeenKeyID(), false);
            aiController->m_profiler.stopProfilingScope("DETECT");
            return;
        }

        if ((aiController->GetPawn()->GetActorLocation() - playerCharacter->GetActorLocation()).Size() < 500.f) {
            OwnerComp.GetBlackboardComponent()->SetValue<UBlackboardKeyType_Bool>(aiController->GetTargetSeenKeyID(), true);
            GEngine->AddOnScreenDebugMessage(0, 2, FColor::Green, TEXT("Player is seen"));
        }

        else
        {
            OwnerComp.GetBlackboardComponent()->SetValue<UBlackboardKeyType_Bool>(aiController->GetTargetSeenKeyID(), false);
            aiController->m_profiler.stopProfilingScope("DETECT");
        }
    }
}

//    if (ASoftDesignTrainingCharacter* aiBase = Cast<ASoftDesignTrainingCharacter>(aiController->GetCharacter()))
//    {
//        aiBase->DetectPlayer();
//        if (aiBase->IsTargetPlayerSeen())
//        {
//            //write to bb that the player is seen
//            OwnerComp.GetBlackboardComponent()->SetValue<UBlackboardKeyType_Bool>(aiController->GetTargetSeenKeyID(), true);

//            //write to bb the position of the target
//            OwnerComp.GetBlackboardComponent()->SetValue<UBlackboardKeyType_Vector>(aiController->GetTargetPosBBKeyID(), aiBase->GetTargetPlayerPos());
//        }
//        else
//        {
//            //write to bb that the player is not seen
//            OwnerComp.GetBlackboardComponent()->SetValue<UBlackboardKeyType_Bool>(aiController->GetTargetSeenKeyID(), false);
//        }

//    }
//}
