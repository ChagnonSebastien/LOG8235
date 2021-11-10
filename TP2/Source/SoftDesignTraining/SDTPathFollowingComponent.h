// Fill out your copyright notice in the Description page of Project Settings.
/*
 Authors:
   - Sébastien Chagnon (1804702)
   - Andy Lam (1960209)
   - Jason Thai (1956172)
   - Alexandre Ramtoula (1959456)
   - Philippe Trempe (1686069)
*/
#pragma once

#include "CoreMinimal.h"
#include "Navigation/PathFollowingComponent.h"
#include "SDTPathFollowingComponent.generated.h"



/**
* A component resposible for path following.
*/
UCLASS(ClassGroup = AI, config = Game)
class SOFTDESIGNTRAINING_API USDTPathFollowingComponent : public UPathFollowingComponent
{
    GENERATED_UCLASS_BODY()

public:
    virtual void FollowPathSegment(float deltaTime) override;
    virtual void SetMoveSegment(int32 segmentStartIndex) override;

};
