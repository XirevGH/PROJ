// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbilityTargetActor_Trace.h"
#include "GATA_GroundTrace_Indicator.generated.h"

/**
 * 
 */
UCLASS()
class PROJ_API AGATA_GroundTrace_Indicator : public AGameplayAbilityTargetActor_Trace
{
	GENERATED_BODY()
	protected:
	void BeginPlay() override;

public:
	AGATA_GroundTrace_Indicator();
	
	virtual FHitResult PerformTrace(AActor* InSourceActor) override;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UDecalComponent* Decal;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	USceneComponent* Root;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta = (ExposeOnSpawn = true), Category = Trace)
	FVector Size;

	UPROPERTY(BlueprintReadOnly)
	FVector TargetLocation;
};
