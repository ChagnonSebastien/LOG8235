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
        ACharacter* playerCharacter = UGameplayStatics::GetPlayerCharacter(GetWorld(), 0);
        if (!playerCharacter) {
            OwnerComp.GetBlackboardComponent()->SetValue<UBlackboardKeyType_Bool>(aiController->GetTargetSeenKeyID(), false);
            return;
        }

        if ((aiController->GetPawn()->GetActorLocation() - playerCharacter->GetActorLocation()).Size() < 500.f) {
            OwnerComp.GetBlackboardComponent()->SetValue<UBlackboardKeyType_Bool>(aiController->GetTargetSeenKeyID(), true);
        }

        else
        {
            OwnerComp.GetBlackboardComponent()->SetValue<UBlackboardKeyType_Bool>(aiController->GetTargetSeenKeyID(), false);
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
