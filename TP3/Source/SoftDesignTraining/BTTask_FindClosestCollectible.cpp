// Fill out your copyright notice in the Description page of Project Settings.


#include "BTTask_FindClosestCollectible.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Runtime/NavigationSystem/Public/NavigationSystem.h"
#include "SDTAIController.h"
#include "SDTCollectible.h"
#include "BehaviorTree/Blackboard/BlackBoardKeyType.h"
#include "BehaviorTree/Blackboard/BlackboardKeyType_Vector.h"
#include "Kismet/GameplayStatics.h"

UBTTask_FindClosestCollectible::UBTTask_FindClosestCollectible(FObjectInitializer const& ObjectInitializer)
{
    NodeName = TEXT("Find Random Collectible");
}

EBTNodeResult::Type UBTTask_FindClosestCollectible::ExecuteTask(UBehaviorTreeComponent& owner_comp, uint8* node_memory)
{
    GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Purple, FString::Printf(TEXT("Behavior Tick")));

    auto const cont = Cast<ASDTAIController>(owner_comp.GetAIOwner());
    auto const npc = cont->GetPawn();

    float closestSqrCollectibleDistance = 18446744073709551610.f;
    ASDTCollectible* closestCollectible = nullptr;

    TArray<AActor*> foundCollectibles;
    UGameplayStatics::GetAllActorsOfClass(GetWorld(), ASDTCollectible::StaticClass(), foundCollectibles);

    while (foundCollectibles.Num() != 0)
    {
        int index = FMath::RandRange(0, foundCollectibles.Num() - 1);

        ASDTCollectible* collectibleActor = Cast<ASDTCollectible>(foundCollectibles[index]);
        if (!collectibleActor)
        {
            FinishLatentTask(owner_comp, EBTNodeResult::Failed);
            return EBTNodeResult::Failed;
        }

        if (!collectibleActor->IsOnCooldown())
        {
            cont->get_blackboard()->SetValueAsVector(GetSelectedBlackboardKey(), collectibleActor->GetActorLocation());
            FinishLatentTask(owner_comp, EBTNodeResult::Succeeded);
            return EBTNodeResult::Succeeded;
        }
        else
        {
            foundCollectibles.RemoveAt(index);
        }
    }

    FinishLatentTask(owner_comp, EBTNodeResult::Failed);
    return EBTNodeResult::Failed;
}
