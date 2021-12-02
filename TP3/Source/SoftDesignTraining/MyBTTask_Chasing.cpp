// Fill out your copyright notice in the Description page of Project Settings.
#include "MyBTTask_Chasing.h"
#include "BehaviorTree/Blackboard/BlackboardKeyType_Vector.h"

EBTNodeResult::Type UMyBTTask_Chasing::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	if (ASDTAIController* aiController = Cast<ASDTAIController>(OwnerComp.GetAIOwner())) {

		ACharacter* playerCharacter = UGameplayStatics::GetPlayerCharacter(GetWorld(), 0);
		if (!playerCharacter) {
			GEngine->AddOnScreenDebugMessage(0, 2, FColor::Green, TEXT("Should not chase1"));
			aiController->m_profiler.stopProfilingScope("DETECT");
			return EBTNodeResult::Failed;
		}
		
		OwnerComp.GetBlackboardComponent()->SetValue<UBlackboardKeyType_Vector>(aiController->GetTargetPosBBKeyID(), playerCharacter->GetActorLocation());
		aiController->MoveToLocation(playerCharacter->GetActorLocation(), 0.5f, false, true, true, NULL, false);
		//GEngine->AddOnScreenDebugMessage(0, 2, FColor::Green, TEXT("Should chase"));
		aiController->m_profiler.stopProfilingScope("DETECT");
		return EBTNodeResult::Succeeded;
	}
	GEngine->AddOnScreenDebugMessage(0, 2, FColor::Green, TEXT("Should not chase3"));
	return EBTNodeResult::Failed;
}