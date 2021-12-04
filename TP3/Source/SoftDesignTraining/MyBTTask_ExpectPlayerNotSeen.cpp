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
				playerSeen = groupManager->IsPlayerDetected();

				if (playerSeen) {

					FVector lkp = groupManager->GetAssignedPos(GetWorld(), aiController);
					FVector actorLocation = aiController->GetPawn()->GetActorLocation();
					ACharacter* playerCharacter = UGameplayStatics::GetPlayerCharacter(GetWorld(), 0);

					if ((lkp - actorLocation).Size() < 50.0) {
						groupManager->UnregisterAIAgent(aiController);
						playerSeen = false;
					}
					else if ( playerCharacter && (playerCharacter->GetActorLocation() - actorLocation).Size() <= 250.0) {

						OwnerComp.GetBlackboardComponent()->SetValue<UBlackboardKeyType_Vector>(aiController->GetTargetPlayerLocationKeyID(), playerCharacter->GetActorLocation());
					}
					else {
						OwnerComp.GetBlackboardComponent()->SetValue<UBlackboardKeyType_Vector>(aiController->GetTargetPlayerLocationKeyID(), lkp);
					}
				}
			}
			else {
				
				if (playerSeen) {
					groupManager->RegisterAIAgent(aiController);
					ACharacter* playerCharacter = UGameplayStatics::GetPlayerCharacter(GetWorld(), 0);
					if (playerCharacter) {
						OwnerComp.GetBlackboardComponent()->SetValue<UBlackboardKeyType_Vector>(aiController->GetTargetPlayerLocationKeyID(), playerCharacter->GetActorLocation());
					}
				}

			}
		}

		return playerSeen ? EBTNodeResult::Failed : EBTNodeResult::Succeeded;

	}
	return EBTNodeResult::Failed;


}

