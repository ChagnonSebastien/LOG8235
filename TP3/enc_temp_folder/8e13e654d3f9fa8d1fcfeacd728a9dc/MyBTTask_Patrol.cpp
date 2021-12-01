// Fill out your copyright notice in the Description page of Project Settings.


#include "MyBTTask_Patrol.h"

EBTNodeResult::Type UMyBTTask_Patrol::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	if (ASDTAIController* aiController = Cast<ASDTAIController>(OwnerComp.GetAIOwner())) {
		
		auto const test1 = OwnerComp.GetBlackboardComponent()->GetValue<UBlackboardKeyType_Bool>(aiController->GetReachedTargetKeyID());
		GEngine->AddOnScreenDebugMessage(0, 2, FColor::Green, FString::Printf(TEXT("Test %f"), test1));


		if (OwnerComp.GetBlackboardComponent()->GetValue<UBlackboardKeyType_Bool>(aiController->GetReachedTargetKeyID())) {
			
			float closestSqrCollectibleDistance = 18446744073709551610.f;
			ASDTCollectible* closestCollectible = nullptr;

			TArray<AActor*> foundCollectibles;
			UGameplayStatics::GetAllActorsOfClass(GetWorld(), ASDTCollectible::StaticClass(), foundCollectibles);

			while (foundCollectibles.Num() != 0)
			{
				int index = FMath::RandRange(0, foundCollectibles.Num() - 1);

				ASDTCollectible* collectibleActor = Cast<ASDTCollectible>(foundCollectibles[index]);
				if (!collectibleActor) {
					GEngine->AddOnScreenDebugMessage(0, 2, FColor::Green, TEXT("testing1"));
					return EBTNodeResult::Failed;
				}

				if (!collectibleActor->IsOnCooldown())
				{
					aiController->MoveToLocation(foundCollectibles[index]->GetActorLocation(), 0.5f, false, true, true, NULL, false);

					OwnerComp.GetBlackboardComponent()->SetValue<UBlackboardKeyType_Bool>(aiController->GetReachedTargetKeyID(), false);
					return EBTNodeResult::Succeeded;
				}
				else
				{
					foundCollectibles.RemoveAt(index);
				}
			}

		}
	}
	return EBTNodeResult::Failed;
}
