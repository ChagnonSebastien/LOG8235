// Fill out your copyright notice in the Description page of Project Settings.


#include "MyBTTask_ExpectPlayerNotSeen.h"
#include "BehaviorTree/Blackboard/BlackboardKeyType_Bool.h"
#include "SoftDesignTraining.h"
#include "SDTUtils.h"
#include "AiAgentGroupManager.h"
#include "SoftDesignTrainingCharacter.h"

EBTNodeResult::Type UMyBTTask_ExpectPlayerNotSeen::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	if (ASDTAIController* aiController = Cast<ASDTAIController>(OwnerComp.GetAIOwner()))
	{

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


}

