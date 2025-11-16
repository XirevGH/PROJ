// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbilityTargetActor_GroundTrace.h"
#include "GATA_GroundTrace_Indicator.generated.h"

/**
 * 
 */
UCLASS()
class PROJ_API AGATA_GroundTrace_Indicator : public AGameplayAbilityTargetActor_GroundTrace
{
	GENERATED_BODY()

public:
	AGATA_GroundTrace_Indicator();

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UDecalComponent* Decal;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	USceneComponent* Root;
	
};
