// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Engine/DataAsset.h"
#include "AbilityData.generated.h"

class UGameplayEffect;
/**
 * 
 */

UENUM(BlueprintType)
enum class EEffectApplicationPolicy : uint8
{
	ApplyToTarget     UMETA(DisplayName = "Apply To Target"),
	ApplyToSelf       UMETA(DisplayName = "Apply To Self"),
	ApplyToBoth       UMETA(DisplayName = "Apply To Self & Target")
};
USTRUCT(BlueprintType)
struct FAttackEffectEntry
{
	GENERATED_BODY()

public:

	// Which GameplayEffect to apply
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSubclassOf<UGameplayEffect> Effect = nullptr;

	// SetByCaller tag magnitude
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TMap<FGameplayTag, float> SetByCallerValues;

	//Apply effect policy (who want the effect to ☠️effect☠️)
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	EEffectApplicationPolicy ApplicationPolicy = EEffectApplicationPolicy::ApplyToTarget;
};

UCLASS()
class PROJ_API UAbilityData : public UPrimaryDataAsset
{
	GENERATED_BODY()
public:
	/*Montage*/
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	UAnimMontage* Montage;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	float NotifyBeginTime;
	
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
