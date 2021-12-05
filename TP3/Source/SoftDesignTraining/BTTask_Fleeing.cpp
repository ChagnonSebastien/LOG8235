// Fill out your copyright notice in the Description page of Project Settings.


#include "BTTask_Fleeing.h"

EBTNodeResult::Type UBTTask_Fleeing::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	ASDTAIController* aiController = Cast<ASDTAIController>(OwnerComp.GetAIOwner());
	aiController->m_profiler.startProfilingScope("FLEE");

    float bestLocationScore = 0.f;
    ASDTFleeLocation* bestFleeLocation = nullptr;

    ACharacter* playerCharacter = UGameplayStatics::GetPlayerCharacter(GetWorld(), 0);
    if (!playerCharacter)
        throw "This method should not be called in the first place if the player is not playing.";

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

        }
    }

	OwnerComp.GetBlackboardComponent()->SetValueAsVector(GetSelectedBlackboardKey(), bestFleeLocation->GetActorLocation());

	aiController->m_profiler.stopProfilingScope("FLEE");
	aiController->budget->LogExecutionTime(FString(TEXT("FLEE")), aiController->m_profiler.getScopeElapsedSeconds("FLEE"));

	return EBTNodeResult::Succeeded;
}