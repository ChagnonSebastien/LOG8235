// Fill out your copyright notice in the Description page of Project Settings.

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

private:
    bool IsSegmentNavigationLink(int32 segmentStartIndex);
    void CalculateJumpDistance(FVector startLocation, FVector endLocation);
};
