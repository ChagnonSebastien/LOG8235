/*
* AiAgentGroupManager.cpp
* Authors:
	- Sebastien Chagnon
	- Andy Lam
	- Jason Thai
	- Alexandre Ramtoula
	- Philippe Trempe
*/
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

	/// <summary>
	/// The target time of the Budget in seconds. A bigger value allows more AIs logic to be run each frame.
	/// </summary>
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = AI)
	float TargetTime = 0.01f;

	/// <summary>
	/// A readonly variable showing through the blueprints the amount of AIs allowed to run each tick.
	/// </summary>
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
