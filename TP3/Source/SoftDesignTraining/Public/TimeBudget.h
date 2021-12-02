// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Containers/Queue.h"
#include "Containers/Set.h"
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
	float TargetTime = 0.05f;

	void registerController(int id);
	bool requestAllocation(int id);
	void notifyTime(float time);

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

private:
	TQueue<int> controllers;
	TSet<int> activeControllers;

	int amountData = 0;
	float computeTime = 0;

};
