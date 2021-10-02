// Copyright 1998-2015 Epic Games, Inc. All Rights Reserved.
#pragma once
#include "GameFramework/GameMode.h"
#include "ScoreUserWidget.h"
#include "SoftDesignTrainingGameMode.generated.h"

UCLASS(minimalapi)
class ASoftDesignTrainingGameMode : public AGameMode
{
	GENERATED_BODY()

protected:
	UPROPERTY(EditAnywhere, Category="Class Types")
	TSubclassOf<UUserWidget> WidgetClass;

	UPROPERTY(VisibleInstanceOnly, Category = "Runtime")
	class UScoreUserWidget* ScoreWidget;

public:
	ASoftDesignTrainingGameMode();

	virtual void BeginPlay() override;
	virtual void Tick(float DeltaSeconds) override;
};



