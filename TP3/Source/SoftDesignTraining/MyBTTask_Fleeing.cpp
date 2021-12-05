// Fill out your copyright notice in the Description page of Project Settings.


#include "MyBTTask_Fleeing.h"
#include "BehaviorTree/Blackboard/BlackboardKeyType_Vector.h"

EBTNodeResult::Type UMyBTTask_Fleeing::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	if (ASDTAIController* aiController = Cast<ASDTAIController>(OwnerComp.GetAIOwner())) {
        float bestLocationScore = 0.f;
        ASDTFleeLocation* bestFleeLocation = nullptr;

        ACharacter* playerCharacter = UGameplayStatics::GetPlayerCharacter(GetWorld(), 0);
        if (!playerCharacter)
            return EBTNodeResult::Failed;

        for (TActorIterator<ASDTFleeLocation> actorIterator(GetWorld(), ASDTFleeLocation::StaticClass()); actorIterator; ++actorIterator)
        {
            ASDTFleeLocation* fleeLocation = Cast<ASDTFleeLocation>(*actorIterator);
            if (fleeLocation)
            {
                float distToFleeLocation = FVector::Dist(fleeLocation->GetActorLocation(), playerCharacter->GetActorLocation());

                FVector selfToPlayer = playerCharacter->GetActorLocation() - aiController->GetPawn()->GetActorLocation();
                selfToPlayer.Normalize();

                FVector selfToFleeLocation = fleeLocation->GetActorLocation() - aiController->GetPawn()->GetActorLocation();
                selfToFleeLocation.Normalize();

                float fleeLocationToPlayerAngle = FMath::RadiansToDegrees(acosf(FVector::DotProduct(selfToPlayer, selfToFleeLocation)));
                float locationScore = distToFleeLocation + fleeLocationToPlayerAngle * 100.f;

                if (locationScore > bestLocationScore)
                {
                    bestLocationScore = locationScore;
                    bestFleeLocation = fleeLocation;
                }

                DrawDebugString(GetWorld(), FVector(0.f, 0.f, 10.f), FString::SanitizeFloat(locationScore), fleeLocation, FColor::Red, 5.f, false);
            }
        }

        if (bestFleeLocation)
        {
            OwnerComp.GetBlackboardComponent()->SetValue<UBlackboardKeyType_Vector>(aiController->GetTargetFleeLocationKeyID(), bestFleeLocation->GetActorLocation());
            aiController->MoveToLocation(bestFleeLocation->GetActorLocation(), 0.5f, false, true, false, NULL, false);
        }
		return EBTNodeResult::Succeeded;
	}
	return EBTNodeResult::Failed;
}