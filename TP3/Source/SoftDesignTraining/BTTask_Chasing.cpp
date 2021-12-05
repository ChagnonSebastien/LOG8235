// Fill out your copyright notice in the Description page of Project Settings.
#include "BTTask_Chasing.h"
#include "BehaviorTree/Blackboard/BlackboardKeyType_Vector.h"

EBTNodeResult::Type UBTTask_Chasing::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
    ASDTAIController* aiController = Cast<ASDTAIController>(OwnerComp.GetAIOwner());
    aiController->m_profiler.startProfilingScope("DETECT");

    AiAgentGroupManager* groupManager = AiAgentGroupManager::GetInstance();

    try {
        FVector targetLocation = groupManager->GetAssignedPos(aiController->GetWorld(), aiController->id, aiController->GetPawn()->GetActorLocation());
        OwnerComp.GetBlackboardComponent()->SetValueAsVector(GetSelectedBlackboardKey(), targetLocation);
    }
    catch (...) {
        // Ignore. Keep going to last valid target.
    }
    DrawDebugSphere(aiController->GetWorld(), aiController->GetPawn()->GetActorLocation(), 25.0f, 32, FColor::Red, false, 0.05);

    aiController->m_profiler.stopProfilingScope("DETECT");
    aiController->budget->LogExecutionTime(FString(TEXT("DETECT")), aiController->m_profiler.getScopeElapsedSeconds("DETECT"));

    return EBTNodeResult::Succeeded;
}