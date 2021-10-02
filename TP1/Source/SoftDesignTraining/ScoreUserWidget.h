// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/Button.h"
#include "Components/TextBlock.h"
#include "ScoreUserWidget.generated.h"

/**
 * 
 */
UCLASS()
class SOFTDESIGNTRAINING_API UScoreUserWidget : public UUserWidget
{
	GENERATED_BODY()

protected:
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
		class UTextBlock* ScoreValue;

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
		class UTextBlock* DeathValue;

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
		class UTextBlock* Time;

	float totalTime = 0;

public:
	void SetScore(int score);
	void SetDeaths(int deaths);
	void IncreaseTime(float time);

	virtual bool Initialize() override;
	virtual void NativeConstruct() override;
};
