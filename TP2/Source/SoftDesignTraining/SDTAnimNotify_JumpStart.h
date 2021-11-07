// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotify.h"
#include "SDTAnimNotify_JumpStart.generated.h"

/**
 * An animation notification that signals jump start.
 */
UCLASS()
class SOFTDESIGNTRAINING_API USDTAnimNotify_JumpStart : public UAnimNotify
{
	GENERATED_BODY()
	
    virtual void Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation) override;
};
