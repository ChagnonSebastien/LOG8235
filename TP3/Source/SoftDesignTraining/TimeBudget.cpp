// Fill out your copyright notice in the Description page of Project Settings.


#include "TimeBudget.h"
#include "SoftDesignTraining.h"

// Sets default values
ATimeBudget::ATimeBudget()
{
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void ATimeBudget::BeginPlay()
{
	Super::BeginPlay();
	timeLog.Add(FString(TEXT("UPDATE")), TArray<double>());
	timeLog.Add(FString(TEXT("DETECT")), TArray<double>());
	timeLog.Add(FString(TEXT("FLEE")), TArray<double>());
	timeLog.Add(FString(TEXT("COLLECT")), TArray<double>());
}

// Called every frame
void ATimeBudget::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	for (int i = 9; i >= 0; i -= 1) {
		int element = controllers[i];
		if (activeControllers.Contains(element)) {
			controllers.RemoveAt(i, 1, true);
			controllers.Add(element);
		}
	}
	activeControllers.Empty();

	double totalTime = 0;
	TArray<FString> keys;
	timeLog.GetKeys(keys);
	for (auto key : keys) {
		TArray<double>* target = timeLog.Find(key);

		double totalScopeTime = 0;
		for (auto log : *target) {
			totalScopeTime += log;
		}

		totalTime += totalScopeTime;
		GEngine->AddOnScreenDebugMessage(-1, DeltaTime, FColor::Blue, FString::Printf(TEXT("%s ran %d times and took %f seconds."), *key, target->Num(), totalScopeTime));
		target->Empty();
	}

	GEngine->AddOnScreenDebugMessage(-1, DeltaTime, FColor::Blue, FString::Printf(TEXT("Total Time: %f seconds."), totalTime));

	if (totalTime < TargetTime)
	{
		if (m_acceptAmountPerFrame < controllers.Num())
		{
			m_acceptAmountPerFrame += 1;
		}
	}
	else
	{
		m_acceptAmountPerFrame -= 1;
	}
}

void ATimeBudget::registerController(int id)
{
	controllers.Add(id);
}

bool ATimeBudget::requestAllocation(int id)
{
	int index = controllers.IndexOfByPredicate([id](int element){ return element == id; });
	bool accept = index < controllers.Num();
	if (accept) {
		activeControllers.Add(id);
	}
	return accept;
}

void ATimeBudget::LogExecutionTime(FString scope, double time)
{
	TArray<double>* target = timeLog.Find(scope);
	target->Add(time);
}

