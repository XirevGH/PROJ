// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilityData.h"
#include "Engine/DataAsset.h"
#include "AbilityDataRegistry.generated.h"

/**
 * 
 */
UCLASS()
class PROJ_API UAbilityDataRegistry : public UPrimaryDataAsset
{
	GENERATED_BODY()
public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	TMap<FGameplayTag, UAbilityData*> AbilityMap;
};
