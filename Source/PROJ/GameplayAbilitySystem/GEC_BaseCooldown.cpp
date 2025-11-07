// Fill out your copyright notice in the Description page of Project Settings.


#include "GEC_BaseCooldown.h"
#include "GameplayTagContainer.h"


UGEC_BaseCooldown::UGEC_BaseCooldown(float InDuration, const FName& InCooldownTagName)
{
	DurationPolicy = EGameplayEffectDurationType::HasDuration;
	DurationMagnitude = FScalableFloat(InDuration);

	if (InCooldownTagName != NAME_None)
	{
		CooldownTags.AddTag(FGameplayTag::RequestGameplayTag(InCooldownTagName));
	}
}

UGEC_BaseCooldown::UGEC_BaseCooldown(const FObjectInitializer& ObjectInitializer)
{
	DurationPolicy = EGameplayEffectDurationType::HasDuration;
	DurationMagnitude = FScalableFloat(3.f);
}

const FGameplayTagContainer& UGEC_BaseCooldown::GetCooldownTags(TSubclassOf<UGEC_BaseCooldown> CooldownEffectClass)
{
	if (!CooldownEffectClass) return FGameplayTagContainer::EmptyContainer;

	UGEC_BaseCooldown* DefaultObj = CooldownEffectClass->GetDefaultObject<UGEC_BaseCooldown>();
	if (DefaultObj)
	{
		return DefaultObj->CooldownTags;
	}

	return FGameplayTagContainer::EmptyContainer;
}
