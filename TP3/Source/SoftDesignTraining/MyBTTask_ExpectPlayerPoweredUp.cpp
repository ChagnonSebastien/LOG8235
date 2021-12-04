// Fill out your copyright notice in the Description page of Project Settings.


#include "MyBTTask_ExpectPlayerPoweredUp.h"
#include "SoftDesignTraining.h"
#include "SDTUtils.h"
#include "SoftDesignTrainingCharacter.h"

EBTNodeResult::Type UMyBTTask_ExpectPlayerPoweredUp::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{

    if (SDTUtils::IsPlayerPoweredUp(GetWorld()))
    {
        return EBTNodeResult::Succeeded;
    }

    return EBTNodeResult::Failed;
}

