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
		//ACharacter* playerCharacter = UGameplayStatics::GetPlayerCharacter(GetWorld(), 0);
		//if (!playerCharacter) {
		//    OwnerComp.GetBlackboardComponent()->SetValue<UBlackboardKeyType_Bool>(aiController->GetTargetSeenKeyID(), false);
		//    return;
		//}

		//aiController->UpdatePlayerInteraction(DeltaSeconds);

		bool playerSeen = aiController->IsPlayerSeen();
		AiAgentGroupManager* groupManager = AiAgentGroupManager::GetInstance();

		if (groupManager) {

			if (groupManager->IsAgentInGroup(aiController)) {
				bool playerDetectedByGroup = groupManager->IsPlayerDetected();
				OwnerComp.GetBlackboardComponent()->SetValue<UBlackboardKeyType_Bool>(aiController->GetTargetSeenKeyID(), playerDetectedByGroup);

				if (playerDetectedByGroup) {
					GEngine->AddOnScreenDebugMessage(0, 2, FColor::Green, TEXT("Player is seen by Group"));

					FVector lkp = groupManager->GetAssignedPos(GetWorld(), aiController);
					FVector actorLocation = aiController->GetPawn()->GetActorLocation();
					if ((lkp - actorLocation).Size() < 50.0) {
						GEngine->AddOnScreenDebugMessage(0, 2, FColor::Green, TEXT("PositionReached"));
						groupManager->UnregisterAIAgent(aiController);
						aiController->InvalidateTargetLKPInfo();
						OwnerComp.GetBlackboardComponent()->SetValue<UBlackboardKeyType_Bool>(aiController->GetTargetSeenKeyID(), false);
					}
					else if ((lkp - actorLocation).Size() < 100.0) {
						ACharacter* playerCharacter = UGameplayStatics::GetPlayerCharacter(GetWorld(), 0);
						if (playerCharacter) {
							OwnerComp.GetBlackboardComponent()->SetValue<UBlackboardKeyType_Vector>(aiController->GetTargetPlayerLocationKeyID(), playerCharacter->GetActorLocation());
						}
					}
					else {
						OwnerComp.GetBlackboardComponent()->SetValue<UBlackboardKeyType_Vector>(aiController->GetTargetPlayerLocationKeyID(), lkp);
					}
				}
			}
			else {
				GEngine->AddOnScreenDebugMessage(0, 2, FColor::Green, TEXT("Agent not in group"));
				OwnerComp.GetBlackboardComponent()->SetValue<UBlackboardKeyType_Bool>(aiController->GetTargetSeenKeyID(), playerSeen);
				
				if (playerSeen) {
					groupManager->RegisterAIAgent(aiController);
					ACharacter* playerCharacter = UGameplayStatics::GetPlayerCharacter(GetWorld(), 0);
					if (playerCharacter) {
						OwnerComp.GetBlackboardComponent()->SetValue<UBlackboardKeyType_Vector>(aiController->GetTargetPlayerLocationKeyID(), playerCharacter->GetActorLocation());
					}
				}

			}
		}
		
	}
	else OwnerComp.GetBlackboardComponent()->SetValue<UBlackboardKeyType_Bool>(aiController->GetTargetSeenKeyID(), false);

	//if ((aiController->GetPawn()->GetActorLocation() - playerCharacter->GetActorLocation()).Size() < 500.f) {
	//    OwnerComp.GetBlackboardComponent()->SetValue<UBlackboardKeyType_Bool>(aiController->GetTargetSeenKeyID(), true);
	//    GEngine->AddOnScreenDebugMessage(0, 2, FColor::Green, TEXT("Player is seen"));
	//}

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
