// Fill out your copyright notice in the Description page of Project Settings.


#include "BTTask_SDTMoveTo.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Runtime/NavigationSystem/Public/NavigationSystem.h"
#include "SDTAIController.h"
#include "SDTCollectible.h"
#include "BehaviorTree/Blackboard/BlackBoardKeyType.h"
#include "BehaviorTree/Blackboard/BlackboardKeyType_Vector.h"
#include "Kismet/GameplayStatics.h"

UBTTask_SDTMoveTo::UBTTask_SDTMoveTo(FObjectInitializer const& ObjectInitializer)
{
    NodeName = TEXT("Move To Target");
}

EBTNodeResult::Type UBTTask_SDTMoveTo::ExecuteTask(UBehaviorTreeComponent& owner_comp, uint8* node_memory)
{
    auto const cont = Cast<ASDTAIController>(owner_comp.GetAIOwner());
    auto const npc = cont->GetPawn();

    auto const target = cont->get_blackboard()->GetValueAsVector(GetSelectedBlackboardKey());
    cont->MoveToLocation(target, 0.5f, false, true, true, NULL, false);
    cont->OnMoveToTarget();

    FinishLatentTask(owner_comp, EBTNodeResult::Succeeded);
    return EBTNodeResult::Succeeded;
}
