// Fill out your copyright notice in the Description page of Project Settings.
#include "MyBTTask_Patrol.h"
#include "BehaviorTree/Blackboard/BlackboardKeyType_Vector.h"


EBTNodeResult::Type UMyBTTask_Patrol::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	if (ASDTAIController* aiController = Cast<ASDTAIController>(OwnerComp.GetAIOwner())) {
		aiController->m_profiler.startProfilingScope("COLLECT");

		float closestSqrCollectibleDistance = 18446744073709551610.f;
		ASDTCollectible* closestCollectible = nullptr;

		TArray<AActor*> foundCollectibles;
		UGameplayStatics::GetAllActorsOfClass(GetWorld(), ASDTCollectible::StaticClass(), foundCollectibles);

		while (foundCollectibles.Num() != 0)
		{
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

			ASDTCollectible* collectibleActor = Cast<ASDTCollectible>(foundCollectibles[index]);
			if (!collectibleActor) {
				aiController->m_profiler.stopProfilingScope("COLLECT");
				return EBTNodeResult::Failed;
			}

			if (!collectibleActor->IsOnCooldown())
			{
				aiController->MoveToLocation(foundCollectibles[index]->GetActorLocation(), 0.5f, false, true, true, NULL, false);
				aiController->m_profiler.stopProfilingScope("COLLECT");
				OwnerComp.GetBlackboardComponent()->SetValue<UBlackboardKeyType_Vector>(aiController->GetTargetCollectingLocationKeyID(), foundCollectibles[index]->GetActorLocation());
				return EBTNodeResult::Succeeded;
			}
			else
			{
				foundCollectibles.RemoveAt(index);
			}
			aiController->m_profiler.stopProfilingScope("COLLECT");
		}
	}
	return EBTNodeResult::Failed;
}