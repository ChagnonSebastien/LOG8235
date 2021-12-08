/*
* AiAgentGroupManager.cpp
* Authors:
	- Sebastien Chagnon
	- Andy Lam
	- Jason Thai
	- Alexandre Ramtoula
	- Philippe Trempe
*/
#include "TimeBudget.h"
#include "SoftDesignTraining.h"


/// <summary>
/// Constroctor. Enable ticks for allocation recalculations.
/// </summary>
ATimeBudget::ATimeBudget()
{
	PrimaryActorTick.bCanEverTick = true;

}


/**
* Function called at the begining of the game. Add the allocation arrays that will be used to store the profiling results
* @param OwnerComp The behavior Tree
* @return Returns node succeeded.
*/
void ATimeBudget::BeginPlay()
{
	Super::BeginPlay();
	timeLog.Add(FString(TEXT("UPDATE")), TArray<double>());
	timeLog.Add(FString(TEXT("DETECT")), TArray<double>());
	timeLog.Add(FString(TEXT("FLEE")), TArray<double>());
	timeLog.Add(FString(TEXT("COLLECT")), TArray<double>());
}

/**
* Called every frame.
* Changes how many behavior trees are allowed to run each frame depending on the time spent on AI during the last frames.
* @param DeltaTime The time last frame took
*/
void ATimeBudget::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// Moving the AIs that ran last tick to the back of the queue
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

		// Add the time of each AIs this tick
		double totalScopeTime = 0;
		for (auto log : *target) {
			totalScopeTime += log;
		}
		totalTime += totalScopeTime;

		// Print scope specific times
		GEngine->AddOnScreenDebugMessage(-1, DeltaTime, FColor::Blue, FString::Printf(TEXT("%s ran %d times and took %f seconds."), *key, target->Num(), totalScopeTime));
		target->Empty();
	}

	// Print global times
	GEngine->AddOnScreenDebugMessage(-1, DeltaTime, FColor::Blue, FString::Printf(TEXT("Total Time: %f seconds."), totalTime));


	if (totalTime < TargetTime)
	{
		// Allow more AIs to be run next tick depending on whether or not the time budget has been reached.
		if (m_acceptAmountPerFrame < controllers.Num())
		{
			m_acceptAmountPerFrame += 1;
		}
	}
	else
	{
		// Decrease the AI allocation acceptance rate if the time limit has been passed.
		m_acceptAmountPerFrame -= 1;
	}
}

/**
* Adds this controller to the controller list so it can be added in the rolling allocation array.
* @param id The unique ID of the requesting controller
*/
void ATimeBudget::registerController(int id)
{
	controllers.Add(id);
}

/**
* Request a time allocation. Will return false depending on the previous AI time usages of the last
* frames and the last time this controller has asked for a time allocation.
* @param id The unique ID of the requesting controller
*/
bool ATimeBudget::requestAllocation(int id)
{
	int index = controllers.IndexOfByPredicate([id](int element){ return element == id; });
	bool accept = index < controllers.Num();
	if (accept) {
		activeControllers.Add(id);
	}
	return accept;
}

/**
* Changes how many behavior trees are allowed to run each frame depending on the time spent on AI during the last frames.
* @param scope The name of the scope for which to declare time
* @param scope The time spent by the AI on this specific scope
*/
void ATimeBudget::LogExecutionTime(FString scope, double time)
{
	TArray<double>* target = timeLog.Find(scope);
	target->Add(time);
}

