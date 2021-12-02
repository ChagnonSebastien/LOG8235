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

	float averageTime = amountData == 0 ? TargetTime : computeTime / (float)amountData;
	int serviceAmount = TargetTime / averageTime;
	if (serviceAmount == 0) serviceAmount = 1;
	//GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, FString::Printf(TEXT("%f %d"), averageTime, serviceAmount));

	activeControllers.Empty();
	for (int i = 0; i < serviceAmount; i+=1) {
		int item;
		controllers.Dequeue(item);
		activeControllers.Add(item);
		controllers.Enqueue(item);
	}

	amountData = 0;
	computeTime = 0;
}

void ATimeBudget::registerController(int id)
{
	controllers.Enqueue(id);
}

bool ATimeBudget::requestAllocation(int id)
{
	return activeControllers.Contains(id);
}

void ATimeBudget::notifyTime(float time)
{
	computeTime += time;
	amountData += 1;
	//GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, FString::Printf(TEXT("Received %f"), time));
}

