// Fill out your copyright notice in the Description page of Project Settings.


#include "TimeBudget.h"
#include "SoftDesignTraining.h"

// Sets default values
ATimeBudget::ATimeBudget()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
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
	GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, FString::Printf(TEXT("Received %d"), activeControllers.Num()));
	activeControllers.Empty();
}

void ATimeBudget::registerController(int id)
{
	controllers.Add(id);
}

bool ATimeBudget::requestAllocation(int id)
{
	int index = controllers.IndexOfByPredicate([id](int element){ return element == id; });
	bool accept = index < 10;
	if (accept) {
		activeControllers.Add(id);
	}
	return accept;
}

void ATimeBudget::notifyTime(float time)
{
	computeTime += time;
	amountData += 1;
	//GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, FString::Printf(TEXT("Received %f"), time));
}

