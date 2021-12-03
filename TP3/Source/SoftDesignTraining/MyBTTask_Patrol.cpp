// Fill out your copyright notice in the Description page of Project Settings.


#include "MyBTTask_Patrol.h"

EBTNodeResult::Type UMyBTTask_Patrol::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	if (ASDTAIController* aiController = Cast<ASDTAIController>(OwnerComp.GetAIOwner())) {
		float closestSqrCollectibleDistance = 18446744073709551610.f;
		ASDTCollectible* closestCollectible = nullptr;

		TArray<AActor*> foundCollectibles;
		UGameplayStatics::GetAllActorsOfClass(GetWorld(), ASDTCollectible::StaticClass(), foundCollectibles);

		while (foundCollectibles.Num() != 0)
		{
			//int index = FMath::RandRange(0, foundCollectibles.Num() - 1);  code de depart pour choisir le collectible est au hasard

			float bestDistance = 1000.f;
			int index = 0;
			for (int i = 0; i <= foundCollectibles.Num() - 1; i++) {
				auto const collectible = Cast<ASDTCollectible>(foundCollectibles[i]);
				float playerDistanceToCollectible = FVector::Dist(collectible->GetActorLocation(), aiController->GetPawn()->GetActorLocation());

				if (playerDistanceToCollectible < bestDistance) {
					bestDistance = playerDistanceToCollectible;
					index = i;
				}
			} // TAKE SECOND BEST

			//int index = foundCollectibles.Num() - 1;
			//GEngine->AddOnScreenDebugMessage(0, 2, FColor::Green, FString::Printf(TEXT("index %f"), index));

			ASDTCollectible* collectibleActor = Cast<ASDTCollectible>(foundCollectibles[index]);
			if (!collectibleActor) {
				return EBTNodeResult::Failed;
			}

			if (!collectibleActor->IsOnCooldown())
			{
				aiController->MoveToLocation(foundCollectibles[index]->GetActorLocation(), 0.5f, false, true, true, NULL, false);
				return EBTNodeResult::Succeeded;
			}
			else
			{
				foundCollectibles.RemoveAt(index);
			}
		}
	}
	return EBTNodeResult::Failed;
}

/*


*/