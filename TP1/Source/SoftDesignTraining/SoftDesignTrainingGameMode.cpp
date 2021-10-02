// Copyright 1998-2015 Epic Games, Inc. All Rights Reserved.

#include "Blueprint/UserWidget.h"
#include "SoftDesignTrainingGameMode.h"
#include "SoftDesignTraining.h"
#include "SoftDesignTrainingPlayerController.h"
#include "SoftDesignTrainingCharacter.h"
#include "SoftDesignTrainingGameState.h"

ASoftDesignTrainingGameMode::ASoftDesignTrainingGameMode()
{
	// use our custom PlayerController class
	PlayerControllerClass = ASoftDesignTrainingPlayerController::StaticClass();

	// use our custom GameState class
	GameStateClass = ASoftDesignTrainingGameState::StaticClass();

	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/Blueprint/BP_SDTMainCharacter"));
	if (PlayerPawnBPClass.Class != NULL)
	{
		DefaultPawnClass = PlayerPawnBPClass.Class;
	}
}

void ASoftDesignTrainingGameMode::BeginPlay()
{
	if (IsValid(WidgetClass))
	{
		ScoreWidget = Cast<UScoreUserWidget>(CreateWidget(GetWorld(), WidgetClass));

		if (ScoreWidget != nullptr)
		{
			ScoreWidget->AddToViewport();
		}
	}
}

void ASoftDesignTrainingGameMode::Tick(float DeltaSeconds)
{
	if (ScoreWidget != nullptr)
	{
		ASoftDesignTrainingGameState* gameState = GetGameState<ASoftDesignTrainingGameState>();
		ScoreWidget->SetScore(gameState->getCollected());
		ScoreWidget->SetDeaths(gameState->getDeaths());
		ScoreWidget->IncreaseTime(DeltaSeconds);
	}
}