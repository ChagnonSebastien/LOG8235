// Fill out your copyright notice in the Description page of Project Settings.

#include "AiAgentGroupManager.h"
#include "SoftDesignTraining.h"
#include "TargetLKPInfo.h"

AiAgentGroupManager* AiAgentGroupManager::m_Instance;

AiAgentGroupManager::AiAgentGroupManager()
{
}

AiAgentGroupManager* AiAgentGroupManager::GetInstance()
{
    if (!m_Instance)
    {
        m_Instance = new AiAgentGroupManager();
    }

    return m_Instance;
}

void AiAgentGroupManager::Destroy()
{
    delete m_Instance;
    m_Instance = nullptr;
}

void AiAgentGroupManager::RegisterAIAgent(ASDTAIController* aiAgent)
{
    m_registeredAgents.AddUnique(aiAgent);
}

void AiAgentGroupManager::UnregisterAIAgent(ASDTAIController* aiAgent)
{
    m_registeredAgents.Remove(aiAgent);
}

bool AiAgentGroupManager::IsPlayerDetected()
{
    int agentCount = m_registeredAgents.Num();
    TargetLKPInfo outLKPInfo = TargetLKPInfo();
    m_targetFound = false;

    for (int i = 0; i < agentCount; ++i)
    {
        ASDTAIController* aiAgent = m_registeredAgents[i];
        if (aiAgent)
        {
            const TargetLKPInfo& targetLKPInfo = aiAgent->GetCurrentTargetLKPInfo();
            if (targetLKPInfo.GetLastUpdatedTimeStamp() > outLKPInfo.GetLastUpdatedTimeStamp())
            {
                m_targetFound = targetLKPInfo.GetLKPState() != TargetLKPInfo::ELKPState::LKPState_Invalid;
                m_targetLkpInfo = targetLKPInfo;
            }
        }
    }

    return m_targetFound;
}

FVector AiAgentGroupManager::GetAssignedPos(UWorld* World, ASDTAIController* aiAgent)
{
    // necessaire?
    //IsPlayerDetected();

    int32 index = 0;
    int agentCount = m_registeredAgents.Num();
    for (int i = 0; i < agentCount; ++i) {
        ASDTAIController* currentAIAgent = m_registeredAgents[i];
        if (aiAgent == currentAIAgent)
        {
            index = i;
            break;
        }
    }

    int nbPoints = m_registeredAgents.Num();
    float angle = 2.f * PI / (float)nbPoints;

    FVector targetLKP = m_targetLkpInfo.GetLKPPos();
    float radius = 300.f;

    targetLKP.X += radius * cos(angle * index);
    targetLKP.Y += radius * sin(angle * index);

    return targetLKP;
}

void AiAgentGroupManager::DrawSphereOnRegisteredAgents(UWorld* World)
{
    int agentCount = m_registeredAgents.Num();
    for (int i = 0; i < agentCount; ++i)
    {
        ASDTAIController* aiAgent = m_registeredAgents[i];
        if (aiAgent)
        {
            auto aiAgentPos = aiAgent->GetPawn()->GetActorLocation();
            DrawDebugSphere(World, aiAgentPos, 25.0f, 32, FColor::Red);
        }
    }
}

bool AiAgentGroupManager::IsAgentInGroup(ASDTAIController* aiAgent) {
    int agentCount = m_registeredAgents.Num();
    for (int i = 0; i < agentCount; ++i)
    {
        ASDTAIController* currentAIAgent = m_registeredAgents[i];
        if (currentAIAgent == aiAgent)
        {
            return true;
        }
    }

    return false;
}