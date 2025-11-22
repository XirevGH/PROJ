// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Engine/DataAsset.h"
#include "AttackData.generated.h"

class UGameplayEffect;
/**
 * 
 */
UCLASS()
class PROJ_API UAttackData : public UPrimaryDataAsset
{
	GENERATED_BODY()
public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	UAnimMontage* Montage;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	TArray<TSubclassOf<UGameplayEffect>> Effects;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	float HitScanInterval = 0.f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	bool bUseHitScan = false;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	float Damage = 0.f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	FGameplayTagContainer GrantedTags;
};
