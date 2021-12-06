/*
* AiAgentGroupManager.h
* Authors:
    - Sebastien Chagnon
    - Andy Lam
    - Jason Thai
    - Alexandre Ramtoula
    - Philippe Trempe
*/
#pragma once

#include "CoreMinimal.h"
#include "NavigationSystem.h"
#include "Containers/Array.h"
#include "DrawDebugHelpers.h"
#include "CoreMinimal.h"

/**
 * AI agent group manager class.
 */
class SOFTDESIGNTRAINING_API AiAgentGroupManager
{
public:
    static AiAgentGroupManager* GetInstance();
    static void Destroy();

    void RegisterAIAgent(int aiAgent);
    void UnregisterAIAgent(int aiAgent);
    void UnregisterAll();
    FVector GetAssignedPos(UWorld* World, int aiAgent, FVector currentLocation);
    bool IsAgentInGroup(int aiAgent);
    FVector GetLKP();
    void UpdateLKP(FVector newLKP);

private:

    //SINGLETON
    AiAgentGroupManager();

    static AiAgentGroupManager* m_Instance;

    TArray<int> m_registeredAgents;
    FVector     m_LKP;
};