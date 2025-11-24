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
	/*Montage*/
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	UAnimMontage* Montage;
	
	/*HitScan*/
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	float HitScanInterval = 0.f;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	bool bUseHitScan = false;
	
	/*Damage*/
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	TSubclassOf<UGameplayEffect> DamageEffect;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	bool bHasDamage;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	float Damage = 0.f;
	
	/*Slow*/
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	TSubclassOf<UGameplayEffect> SlowEffect;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	bool bHasSlow;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	float SlowAmount;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	float SlowDuration;

	/*Stun*/
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	TSubclassOf<UGameplayEffect> StunEffect;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	bool bHasStun;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	float StunDuration;

	/*Tags*/
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	FGameplayTagContainer GrantedTags;
};
