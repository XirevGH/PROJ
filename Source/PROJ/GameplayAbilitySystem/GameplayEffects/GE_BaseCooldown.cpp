// Fill out your copyright notice in the Description page of Project Settings.


#include "GE_BaseCooldown.h"
#include "GameplayTagContainer.h"


UGE_BaseCooldown::UGE_BaseCooldown(float InDuration, const FName& InCooldownTagName)
{
	DurationPolicy = EGameplayEffectDurationType::HasDuration;
	DurationMagnitude = FScalableFloat(InDuration);

	if (InCooldownTagName != NAME_None)
	{
		CooldownTags.AddTag(FGameplayTag::RequestGameplayTag(InCooldownTagName));
	}
}

UGE_BaseCooldown::UGE_BaseCooldown(const FObjectInitializer& ObjectInitializer)
{
	DurationPolicy = EGameplayEffectDurationType::HasDuration;
	DurationMagnitude = FScalableFloat(3.f);
}

const FGameplayTagContainer& UGE_BaseCooldown::GetCooldownTags(TSubclassOf<UGE_BaseCooldown> CooldownEffectClass)
{
	if (!CooldownEffectClass) return FGameplayTagContainer::EmptyContainer;

	UGE_BaseCooldown* DefaultObj = CooldownEffectClass->GetDefaultObject<UGE_BaseCooldown>();
	if (DefaultObj)
	{
		return DefaultObj->CooldownTags;
	}

	return FGameplayTagContainer::EmptyContainer;
}
