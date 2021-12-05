// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Containers/Array.h"
#include "Containers/Set.h"
#include "Containers/Map.h"
#include "GameFramework/Actor.h"
#include "TimeBudget.generated.h"

UCLASS()
class SOFTDESIGNTRAINING_API ATimeBudget : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ATimeBudget();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = AI)
	float TargetTime = 0.01f;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = AI)
	int m_acceptAmountPerFrame = 10;

	void registerController(int id);
	bool requestAllocation(int id);
	void LogExecutionTime(FString scope, double time);

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

private:
	TArray<int> controllers;
	TSet<int> activeControllers;
	TMap<FString, TArray<double>> timeLog;

};
