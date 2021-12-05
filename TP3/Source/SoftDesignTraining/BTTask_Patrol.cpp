// Fill out your copyright notice in the Description page of Project Settings.


#include "BTTask_Patrol.h"

EBTNodeResult::Type UBTTask_Patrol::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	ASDTAIController* aiController = Cast<ASDTAIController>(OwnerComp.GetAIOwner());
	aiController->m_profiler.startProfilingScope("COLLECT");

	TArray<AActor*> foundCollectibles;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), ASDTCollectible::StaticClass(), foundCollectibles);
	foundCollectibles = foundCollectibles.FilterByPredicate([] (AActor* collectible) {
		return !Cast<ASDTCollectible>(collectible)->IsOnCooldown();
	});

	float bestDistance = 1000.f;
	int index = 0;
	for (int i = 0; i <= foundCollectibles.Num() - 1; i++) {
		auto const collectible = Cast<ASDTCollectible>(foundCollectibles[i]);
		float playerDistanceToCollectible = FVector::Dist(collectible->GetActorLocation(), aiController->GetPawn()->GetActorLocation());

		if (playerDistanceToCollectible < bestDistance) {
			bestDistance = playerDistanceToCollectible;
			index = i;
		}
	}

	OwnerComp.GetBlackboardComponent()->SetValueAsVector(GetSelectedBlackboardKey(), Cast<ASDTCollectible>(foundCollectibles[index])->GetActorLocation());

	aiController->m_profiler.stopProfilingScope("COLLECT");
	aiController->budget->LogExecutionTime(FString(TEXT("COLLECT")), aiController->m_profiler.getScopeElapsedSeconds("COLLECT"));

	return EBTNodeResult::Succeeded;
}