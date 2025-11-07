// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayEffect.h"
#include "GEC_BaseCooldown.generated.h"

/**
 * 
 */
UCLASS(Abstract,Blueprintable)
class PROJ_API UGEC_BaseCooldown : public UGameplayEffect
{
	GENERATED_BODY()
	
public:
															//Exempel Cooldown.Dash etc..
	UGEC_BaseCooldown(float InDuration = 3.f, const FName& InCooldownTagName = NAME_None);

	//Default
	UGEC_BaseCooldown(const FObjectInitializer& ObjectInitializer);

	UPROPERTY(BlueprintReadOnly)
	FGameplayTagContainer CooldownTags;

	static const FGameplayTagContainer& GetCooldownTags(TSubclassOf<UGEC_BaseCooldown> CooldownEffectClass);
};
