// Fill out your copyright notice in the Description page of Project Settings.
#include "MyBTTask_Chasing.h"
#include "BehaviorTree/Blackboard/BlackboardKeyType_Vector.h"

EBTNodeResult::Type UMyBTTask_Chasing::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	if (ASDTAIController* aiController = Cast<ASDTAIController>(OwnerComp.GetAIOwner())) {
        FVector targetLocation = OwnerComp.GetBlackboardComponent()->GetValue<UBlackboardKeyType_Vector>(aiController->GetTargetPlayerLocationKeyID());
        aiController->MoveToLocation(targetLocation, 0.5f, false, true, true, NULL, false);
        return EBTNodeResult::Succeeded;
    }
    GEngine->AddOnScreenDebugMessage(0, 2, FColor::Green, TEXT("Not chasing"));
    return EBTNodeResult::Failed;
}