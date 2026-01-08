// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "ProjectileData.h"
#include "Engine/DataAsset.h"
#include "ProjectileDataRegistry.generated.h"

/**
 * 
 */
UCLASS()
class PROJ_API UProjectileDataRegistry : public UPrimaryDataAsset
{
	GENERATED_BODY()

public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	TMap<FGameplayTag, UProjectileData*> ProjectileMap;
};
