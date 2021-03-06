/*
* BTTask_ExpectPlayerNotSeen.cpp
* Authors:
    - Sebastien Chagnon
    - Andy Lam
    - Jason Thai
    - Alexandre Ramtoula
    - Philippe Trempe
*/
#include "BTTask_ExpectPlayerNotSeen.h"
#include "BehaviorTree/Blackboard/BlackboardKeyType_Bool.h"
#include "SoftDesignTraining.h"
#include "SDTUtils.h"
#include "AiAgentGroupManager.h"
#include "SoftDesignTrainingCharacter.h"

/**
* Update profiling scope, check wether AI controller sees the target player, update LKP info, and lastly log execution time.
* @param OwnerComp The behavior Tree
* @return Returns node succeeded if the AI controller sees the target player, else node failed.
*/
EBTNodeResult::Type UBTTask_ExpectPlayerNotSeen::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	ASDTAIController* aiController = Cast<ASDTAIController>(OwnerComp.GetAIOwner());
	aiController->m_profiler.startProfilingScope("DETECT");

	AiAgentGroupManager* groupManager = AiAgentGroupManager::GetInstance();

	bool playerSeen = aiController->IsPlayerSeen();
	if (playerSeen)
	{
		// Join group if not already in it
		groupManager->RegisterAIAgent(aiController->id);

		// Notify group of current player postion
		ACharacter* playerCharacter = UGameplayStatics::GetPlayerCharacter(GetWorld(), 0);
		groupManager->UpdateLKP(playerCharacter->GetActorLocation());
	}
	else if (groupManager->IsAgentInGroup(aiController->id))
	{
		FVector lkp = groupManager->GetLKP();
		if (FVector::Dist2D(lkp, aiController->GetPawn()->GetActorLocation()) > 2000.0f)
		{
			// Agent is too far from the group
			groupManager->UnregisterAIAgent(aiController->id);
		}
		else
		{
			// The group has a sight on the player
			playerSeen = true;
		}
	}

	aiController->m_profiler.stopProfilingScope("DETECT");
	aiController->budget->LogExecutionTime(FString(TEXT("DETECT")), aiController->m_profiler.getScopeElapsedSeconds("DETECT"));

	return playerSeen ? EBTNodeResult::Failed : EBTNodeResult::Succeeded;

}

