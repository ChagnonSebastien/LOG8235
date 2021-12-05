#pragma once

#include "CoreMinimal.h"

UENUM(BlueprintType)
enum Status
{
	Collecting    UMETA(DisplayName = "Collecting"),
	Chassing      UMETA(DisplayName = "Chassing"),
	Fleeing       UMETA(DisplayName = "Fleeing"),
};