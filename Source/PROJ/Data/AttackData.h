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

USTRUCT(BlueprintType)
struct FAttackEffectEntry
{
	GENERATED_BODY()

public:

	// Which GameplayEffect to apply
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSubclassOf<UGameplayEffect> Effect = nullptr;

	// SetByCaller tag → magnitude
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TMap<FGameplayTag, float> SetByCallerValues;
};

UCLASS()
class PROJ_API UAttackData : public UPrimaryDataAsset
{
	GENERATED_BODY()
public:
	/*Montage*/
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	UAnimMontage* Montage;
	
	/*HitScan*/
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	float HitScanInterval = 0.f;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	bool bUseHitScan = false;

	/*Effects & Values from struct*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<FAttackEffectEntry> Effects;

	/*Tags*/
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	FGameplayTagContainer GrantedTags;
};
