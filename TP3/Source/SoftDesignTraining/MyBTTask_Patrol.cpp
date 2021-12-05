// Fill out your copyright notice in the Description page of Project Settings.


#include "MyBTTask_Patrol.h"

EBTNodeResult::Type UMyBTTask_Patrol::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	if (ASDTAIController* aiController = Cast<ASDTAIController>(OwnerComp.GetAIOwner())) {

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

		return EBTNodeResult::Succeeded;
	}
	return EBTNodeResult::Failed;
}