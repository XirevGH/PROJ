// Fill out your copyright notice in the Description page of Project Settings.


#include "BaseGameplayAbility.h"

#include "AbilitySystemComponent.h"
#include "PROJ/GameplayAbilitySystem/GameplayEffects/GE_BaseCooldown.h"



void UBaseGameplayAbility::OnGiveAbility(const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilitySpec& Spec)
{
	Super::OnGiveAbility(ActorInfo, Spec);

	FGameplayTag CooldownTag = GetCooldownTagFromInputID(InputTag);

	ActivationBlockedTags.AddTag(CooldownTag);

	UE_LOG(LogTemp, Warning, TEXT("InputTag  is  %s "), *InputTag.ToString());
	UE_LOG(LogTemp, Warning, TEXT("GetCooldownTagFromInputID Tag is  %s "), *GetCooldownTagFromInputID(InputTag).ToString());
	UE_LOG(LogTemp, Warning, TEXT("ActivationBlockedTags is  %s "), *ActivationBlockedTags.GetByIndex(1).ToString());
}

void UBaseGameplayAbility::ApplyCooldown(const FGameplayAbilitySpecHandle Handle,
                                         const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo) const
{
	Super::ApplyCooldown(Handle, ActorInfo, ActivationInfo);

	if (!CooldownGameplayEffectClass || !ActorInfo) return;

	// Create spec
	FGameplayEffectSpecHandle SpecHandle = MakeOutgoingGameplayEffectSpec(CooldownGameplayEffectClass, GetAbilityLevel());
	if (!SpecHandle.IsValid()) return;

	FGameplayEffectSpec* Spec = SpecHandle.Data.Get();
	if (!Spec) return;
	
	// Set the cooldown duration
	Spec->SetSetByCallerMagnitude(FGameplayTag::RequestGameplayTag(TEXT("Data.Cooldown.Duration")), Cooldown);

	// Add the cooldown tag dynamically so your manager sees it
	const FGameplayTag CooldownTag = GetCooldownTagFromInputID(InputTag);
	Spec->DynamicGrantedTags.AddTag(CooldownTag);
	
	ApplyGameplayEffectSpecToOwner(Handle, ActorInfo, ActivationInfo, SpecHandle);
}

FGameplayTag UBaseGameplayAbility::GetCooldownTagFromInputID(const FGameplayTag InputTag) 
{
	FString TagString = InputTag.ToString();
	TArray<FString> Parts;
	TagString.ParseIntoArray(Parts, TEXT("."), true);

	if (Parts.Num() == 0)
	{
		return FGameplayTag();
	}

	FString Last = Parts.Last(); // "Primary", "Secondary", etc.

	FString CooldownTagString = FString::Printf(TEXT("Cooldown.Ability.%s"), *Last);

	return FGameplayTag::RequestGameplayTag(FName(*CooldownTagString), false);
}
