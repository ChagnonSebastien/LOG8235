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
	amountData = 0;
}

void ATimeBudget::registerController(int id)
{
	controllers.Add(id);
}

bool ATimeBudget::requestAllocation(int id)
{
	amountData += 1;
	int index = controllers.IndexOfByPredicate([id](int element){ return element == id; });
	bool accept = index < controllers.Num();
	if (accept) {
		activeControllers.Add(id);
	}
	return accept;
}

void ATimeBudget::notifyTime(float time)
{
	computeTime += time;
}

