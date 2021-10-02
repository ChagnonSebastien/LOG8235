// Fill out your copyright notice in the Description page of Project Settings.

#include<string>
#include "Blueprint/WidgetTree.h"
#include "ScoreUserWidget.h"

void UScoreUserWidget::SetScore(int score)
{
	ScoreValue->SetText(FText::AsNumber(score));
}

void UScoreUserWidget::SetDeaths(int deaths)
{
	DeathValue->SetText(FText::AsNumber(deaths));
}

void UScoreUserWidget::IncreaseTime(float time)
{
	totalTime += time;
	int totalSeconds = floor(totalTime);

	FFormatNamedArguments args = FFormatNamedArguments();
	args.Add(FString("minutes"), floor(totalSeconds / 60.f));
	args.Add(FString("seconds"), FText::FromString(FString::Printf(TEXT("%02d"), totalSeconds % 60)));
	args.Add(FString("millis"), FText::FromString(FString::Printf(TEXT("%03d"), int(totalTime * 1000) % 1000)));
	Time->SetText(FText::Format(FTextFormat::FromString("{minutes}:{seconds}:{millis}"), args));
}

bool UScoreUserWidget::Initialize() {
	return Super::Initialize();
}

void UScoreUserWidget::NativeConstruct()
{
    Super::NativeConstruct();
}