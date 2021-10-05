/*
* STDAIController.h
* Authors:
    - Sebastien Chagnon
    - Andy Lam
    - Jason Thai
    - Alexandre Ramtoula
    - Philippe Trempe
*/
#pragma once

#include "CoreMinimal.h"
#include "AIController.h"

#include "SDTAIController.generated.h"

/**
 * Class that defines an agent behaviour
 */
UCLASS(ClassGroup = AI, config = Game)
class SOFTDESIGNTRAINING_API ASDTAIController : public AAIController
{
    GENERATED_BODY()
public:

    UPROPERTY(
        EditAnywhere, BlueprintReadWrite, Category = AI, Meta = (
            ShortTooltip = "Afficher le debug",
            ToolTip = "Outil activable permettant de faciliter la comprehension du comportement de l'agent en faisant des affichages lors des simulations"))
        bool debug = false;


    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = AI, Meta = (
        ShortTooltip = "Limite de vision",
        ToolTip = "Determine la distance a laquel les detections d'objets sont faites a l'avant de l'agent"))
        float sightThreshold = 400.f;


    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = AI, Meta = (
        ShortTooltip = "Force du desir",
        ToolTip = "Influence la force du systeme de desir de l'agent intelligent. Plus la valeur est elevee, moins il resistera a l'envie de tourner d'un cote ou de l'autre en mode 'free roam'"))
        float envyStrength = 0.2f;


    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = AI, Meta = (
        ShortTooltip = "Vitesse de rotation",
        ToolTip = "Determine la vitesse a laquelle l'agent fait sa rotation lorsqu'il change de direction"))
        float rotatingSpeed = 3.0f;


    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = AI, Meta=(
        ShortTooltip = "Rayon de la sphere de detection",
        ToolTip = "Determine le rayon de la sphere autour de l'agent lui permettant de detecter les differents objets de son environnement"))
        float detectionSphereRadius = 200.f;

    virtual void Tick(float deltaTime) override;

private:
    FRotator envy = FRotator(0, 1, 0);

    // -1 rotating left, 1 rotating right, 0 not escaping;
    int escapingCorner = 0;

    virtual void computeNearestCollision(float& distance, FVector_NetQuantizeNormal& hitNormal, TArray<struct FHitResult> hits);
    virtual void freeRoam(float& speed, FRotator& walkingDirection, TArray<struct FHitResult> centerHitResults, TArray<struct FHitResult> rightHitResults, TArray<struct FHitResult> leftHitResults, float deltaTime);
    virtual void chaseObject(FRotator& walkingDirection, FVector objectLocation);
    virtual void findCollectible(FHitResult hit, bool& collectibleFound, FVector& collectibleLocation);
    virtual void findPlayer(FHitResult hit, bool& playerFound, FVector& playerLocation, bool& isPlayerPowerUp);
    virtual void sweepForwardObjects(bool& collectibleFound, FVector& collectibleLocation, bool& playerFound, bool& isPlayerPowerUp, FVector& playerLocation);
};
