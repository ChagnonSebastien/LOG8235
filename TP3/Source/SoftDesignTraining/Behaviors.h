// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Behaviors.generated.h"

/**
 * 
 */

UENUM(BlueprintType)
enum EBehaviors
{
	CHASING       UMETA(DisplayName = "Chasing"),
	FLEEING       UMETA(DisplayName = "Fleeing"),
	COLLECTING    UMETA(DisplayName = "Collecting")
};