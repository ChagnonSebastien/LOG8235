/*
* SDTCollectible.cpp
* Authors:
    - Sébastien Chagnon
    - Andy Lam
    - Jason Thai
    - Alexandre Ramtoula
    - Philippe Trempe
*/
#include "SDTCollectible.h"
#include "SoftDesignTraining.h"
#include "SDTUtils.h"

ASDTCollectible::ASDTCollectible()
{
    PrimaryActorTick.bCanEverTick = true;
}

void ASDTCollectible::BeginPlay()
{
    Super::BeginPlay();
    m_initialPosition = GetActorLocation();
    SetToInitialState();
}

void ASDTCollectible::Collect()
{
    GetWorld()->GetTimerManager().SetTimer(m_CollectCooldownTimer, this, &ASDTCollectible::OnCooldownDone, m_CollectCooldownDuration, false);

    GetStaticMeshComponent()->SetVisibility(false);
}

void ASDTCollectible::OnCooldownDone()
{
    GetWorld()->GetTimerManager().ClearTimer(m_CollectCooldownTimer);

    GetStaticMeshComponent()->SetVisibility(true);
    SetToInitialState();
}

bool ASDTCollectible::IsOnCooldown()
{
    return m_CollectCooldownTimer.IsValid();
}
/*
* Name: SetToInitialState
* Description:
    Function which set the collectible to the initial state
* Args: None
* Return: None
*/
void ASDTCollectible::SetToInitialState()
{
    SetActorLocation(m_initialPosition);
    m_speed = FVector(0.f, MAX_SPEED, 0.f);
    m_acceleration = FVector(0.f, MAX_ACCELERATION, 0.f);
}

void ASDTCollectible::Tick(float deltaTime)
{
    Super::Tick(deltaTime);

    if (isMoveable && !IsOnCooldown()) {

        UWorld* world = GetWorld();
        FVector const objectPos = GetActorLocation();

        // deltaT pour atteindre une vitesse nulle
        float deltaT = abs(m_speed.Size() / m_acceleration.Size());
        // xf - xi = v*t + 1/2 * a * t^2 sachant que xi = 0
        float dist = m_speed.Size() * deltaT - 0.5f * m_acceleration.Size() * deltaT * deltaT;
        dist += MIN_DIST_WALL;
        // Check if there is a wall in his direction 
        if (SDTUtils::Raycast(world, FVector(objectPos), FVector(objectPos) + dist * m_acceleration.GetSafeNormal())) {

            //Inverse the orientation of the acceleration
            m_acceleration *= -1.f;
        }

        FVector const displacement = m_speed * deltaTime;
        SetActorLocation(objectPos + displacement, true);
        
        FVector temp_speed = m_speed + m_acceleration * deltaTime;
        m_speed = (MAX_SPEED > abs(temp_speed.Y)) ? temp_speed : (temp_speed.GetSafeNormal() * MAX_SPEED);
    }
}
