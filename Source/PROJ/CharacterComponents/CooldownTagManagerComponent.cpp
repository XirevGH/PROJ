// Fill out your copyright notice in the Description page of Project Settings.


#include "CooldownTagManagerComponent.h"

#include "AbilitySystemComponent.h"

UCooldownTagManagerComponent::UCooldownTagManagerComponent()
{
	PrimaryComponentTick.bCanEverTick = false;

}

void UCooldownTagManagerComponent::Initialize(UAbilitySystemComponent* InASC)
{
	ASC = InASC;

	ASC->OnActiveGameplayEffectAddedDelegateToSelf.AddUObject(
		this, &UCooldownTagManagerComponent::OnActiveGEAdded);

	ASC->OnAnyGameplayEffectRemovedDelegate().AddUObject(
		this, &UCooldownTagManagerComponent::OnActiveGERemoved);
}


void UCooldownTagManagerComponent::OnActiveGEAdded(UAbilitySystemComponent* TargetASC,
	const FGameplayEffectSpec& Spec, FActiveGameplayEffectHandle Handle)
{
	

	FGameplayTagContainer Tags;
	Spec.GetAllGrantedTags(Tags);

	for (const FGameplayTag& Tag : Tags)
	{
		if (!Tag.MatchesTag(FGameplayTag::RequestGameplayTag(TEXT("Cooldown.Ability"))))
			continue;

		int32& Counter = ActiveCooldownCounts.FindOrAdd(Tag);
		Counter++;

		if (Counter == 1)
		{
			ASC->AddLooseGameplayTag(Tag);
		}
	}
}

void UCooldownTagManagerComponent::OnActiveGERemoved(const FActiveGameplayEffect& ActiveGE)
{
	
	FGameplayTagContainer Tags;
	ActiveGE.Spec.GetAllGrantedTags(Tags);

	for (const FGameplayTag& Tag : Tags)
	{
		if (!Tag.MatchesTag(FGameplayTag::RequestGameplayTag(TEXT("Cooldown.Ability"))))
			continue;

		int32& Counter = ActiveCooldownCounts.FindOrAdd(Tag);
		Counter--;

		if (Counter <= 0)
		{
			ASC->RemoveLooseGameplayTag(Tag);
			ActiveCooldownCounts.Remove(Tag);
		}
	}
}

