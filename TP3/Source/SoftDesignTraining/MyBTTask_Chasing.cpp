// Fill out your copyright notice in the Description page of Project Settings.

#include "BehaviorTree/Blackboard/BlackboardKeyType_Bool.h"
#include "MyBTTask_Chasing.h"

EBTNodeResult::Type UMyBTTask_Chasing::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	if (ASDTAIController* aiController = Cast<ASDTAIController>(OwnerComp.GetAIOwner())) {

		ACharacter* playerCharacter = UGameplayStatics::GetPlayerCharacter(GetWorld(), 0);
		if (!playerCharacter)
			return EBTNodeResult::Failed;

		if ((aiController->GetPawn()->GetActorLocation() - playerCharacter->GetActorLocation()).Size() < 300.f) {
			aiController->MoveToLocation(playerCharacter->GetActorLocation(), 0.5f, false, true, true, NULL, false);
		}

		else return EBTNodeResult::Failed; // Go collecting

		return EBTNodeResult::Succeeded;
	}

	return EBTNodeResult::Failed;
}