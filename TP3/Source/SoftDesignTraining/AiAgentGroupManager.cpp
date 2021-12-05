// Fill out your copyright notice in the Description page of Project Settings.

#include "NavigationSystem.h"
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

void AiAgentGroupManager::RegisterAIAgent(int aiAgent)
{
    m_registeredAgents.AddUnique(aiAgent);
}

void AiAgentGroupManager::UnregisterAIAgent(int aiAgent)
{
    m_registeredAgents.Remove(aiAgent);
}

void AiAgentGroupManager::UnregisterAll()
{
    m_registeredAgents.Empty();
}

FVector AiAgentGroupManager::GetAssignedPos(UWorld* World, int aiAgent,  FVector currentLocation)
{
    float distance_to_LKP = FVector::Dist2D(currentLocation, m_LKP);
    if (distance_to_LKP < 50) {
        // Dismiss group if the LKP has been reached
        this->UnregisterAll();
        return m_LKP;
    }

    int index = m_registeredAgents.IndexOfByPredicate([aiAgent] (int value){ return value == aiAgent; });

    
    // Spread agents around the player at a distance of 5m 
    float radius = 500.f;
    int nbPoints = m_registeredAgents.Num();
    float angle = 2.f * PI / (float)nbPoints;
    FVector targetLKP = m_LKP;
    targetLKP.X += radius * cos(angle * index);
    targetLKP.Y += radius * sin(angle * index);

    // The closer you are to the player, the closer the agent is assigned to the player.
    // The goal is for all paths to converge to the player at some point
    float distance_factor = distance_to_LKP / (radius * 2);
    if (distance_factor > 1) distance_factor = 1;
    FVector adjusted_goal = FMath::Lerp(m_LKP, targetLKP, distance_factor);

    // Check if the goal position can be projected onto the NavMesh
    UNavigationSystemV1* navSystem = Cast<UNavigationSystemV1>(World->GetNavigationSystem());
    FNavLocation navLocation;
    bool isTargetPositionNavigable = navSystem->ProjectPointToNavigation(adjusted_goal, navLocation);
    if (isTargetPositionNavigable) {
        return adjusted_goal;
    }

    // It optimal goal is not navigable, path directly to the player
    isTargetPositionNavigable = navSystem->ProjectPointToNavigation(m_LKP, navLocation);
    if (isTargetPositionNavigable) {
        return m_LKP;
    }

    throw "Can't find valid navigable location";
}

bool AiAgentGroupManager::IsAgentInGroup(int aiAgent) {
    return m_registeredAgents.Contains(aiAgent);
}

FVector AiAgentGroupManager::GetLKP()
{
    return m_LKP;
}

void AiAgentGroupManager::UpdateLKP(FVector newLKP)
{
    m_LKP = newLKP;
}