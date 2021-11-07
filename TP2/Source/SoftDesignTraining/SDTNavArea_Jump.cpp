// Fill out your copyright notice in the Description page of Project Settings.

#include "SDTNavArea_Jump.h"
#include "SoftDesignTraining.h"

#include "SDTUtils.h"

/**
* Constructs an USDTNavArea_Jump from an FObjectInitializer.
*
* @param ObjectInitializer The initializer to use to construct the USDTNavArea_Jump.
*/
USDTNavArea_Jump::USDTNavArea_Jump(const FObjectInitializer& ObjectInitializer)
    : Super(ObjectInitializer)
{
    SDTUtils::SetNavTypeFlag(AreaFlags, SDTUtils::Jump);
}
