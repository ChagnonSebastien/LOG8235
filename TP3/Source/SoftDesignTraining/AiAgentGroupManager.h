#pragma once
#include "TargetLKPInfo.h"
#include "SDTAIController.h"
#include "DrawDebugHelpers.h"
#include "CoreMinimal.h"

/**
 *
 */
class SOFTDESIGNTRAINING_API AiAgentGroupManager
{
public:
    static AiAgentGroupManager* GetInstance();
    static void Destroy();

    void RegisterAIAgent(ASDTAIController* aiAgent);
    void UnregisterAIAgent(ASDTAIController* aiAgent);
    void UnregisterAll();
    bool IsPlayerDetected();
    FVector GetAssignedPos(UWorld* World, ASDTAIController* aiAgent);
    void DrawSphereOnRegisteredAgents(UWorld* World);
    bool IsAgentInGroup(ASDTAIController* aiAgent);

private:

    //SINGLETON
    AiAgentGroupManager();

    static AiAgentGroupManager* m_Instance;
    TArray<ASDTAIController*> m_registeredAgents;
    TargetLKPInfo m_targetLkpInfo;
    bool m_targetFound;
};