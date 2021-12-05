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
    NodeName = TEXT("Find Closest Collectible");
}

EBTNodeResult::Type UBTTask_FindClosestCollectible::ExecuteTask(UBehaviorTreeComponent& owner_comp, uint8* node_memory)
{
    auto const cont = Cast<ASDTAIController>(owner_comp.GetAIOwner());
    auto const npc = cont->GetPawn();

    float closestSqrCollectibleDistance = 18446744073709551610.f;
    ASDTCollectible* closestCollectible = nullptr;

    TArray<AActor*> foundCollectibles;
    UGameplayStatics::GetAllActorsOfClass(GetWorld(), ASDTCollectible::StaticClass(), foundCollectibles);
    
    for (AActor* actor: foundCollectibles)
    {
        ASDTCollectible* collectibleActor = Cast<ASDTCollectible>(actor);
        if (collectibleActor->IsOnCooldown()) continue;

        float distance = FVector::Dist2D(npc->GetActorLocation(), actor->GetActorLocation());
        if (distance < closestSqrCollectibleDistance)
        {
            closestSqrCollectibleDistance = distance;
            closestCollectible = collectibleActor;
        }
    }

    if (closestCollectible == nullptr)
    {
        FinishLatentTask(owner_comp, EBTNodeResult::Failed);
        return EBTNodeResult::Failed;
    }

    GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Purple, FString::Printf(TEXT("Behavior Tick")));
    FVector prevTarget = cont->get_blackboard()->GetValueAsVector(GetSelectedBlackboardKey());
    if (FVector::Dist2D(prevTarget, closestCollectible->GetActorLocation()) > 100 || FVector::Dist2D(prevTarget, npc->GetActorLocation()) < 100)
    {
        cont->get_blackboard()->SetValueAsVector(GetSelectedBlackboardKey(), closestCollectible->GetActorLocation());
        cont->MoveToLocation(closestCollectible->GetActorLocation(), 0.5f, false, true, true, NULL, false);
        cont->OnMoveToTarget();
    }

    FinishLatentTask(owner_comp, EBTNodeResult::Succeeded);
    return EBTNodeResult::Succeeded;
}
