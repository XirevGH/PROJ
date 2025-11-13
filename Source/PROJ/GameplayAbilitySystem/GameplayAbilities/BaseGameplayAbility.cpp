// Fill out your copyright notice in the Description page of Project Settings.


#include "BaseGameplayAbility.h"

#include "PROJ/GameplayAbilitySystem/GameplayEffects/GE_BaseCooldown.h"

void UBaseGameplayAbility::ApplyCooldown(const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo) const
{
	Super::ApplyCooldown(Handle, ActorInfo, ActivationInfo);

	if (!CooldownGameplayEffectClass || !ActorInfo)
		return;

	// Create spec
	FGameplayEffectSpecHandle SpecHandle = MakeOutgoingGameplayEffectSpec(CooldownGameplayEffectClass, GetAbilityLevel());
	if (!SpecHandle.IsValid())
		return;

	FGameplayEffectSpec* Spec = SpecHandle.Data.Get();
	if (!Spec)
		return;
	
	Spec->SetSetByCallerMagnitude(FGameplayTag::RequestGameplayTag(TEXT("Data.Cooldown.Duration")), Cooldown);
	UE_LOG(LogTemp, Warning, TEXT("Apply Cooldown to %s "), *ActorInfo->AvatarActor->GetName());
	
	const FGameplayTag& CooldownTag = GetCooldownTagFromInputID(AbilityInputID); // e.g., Cooldown.Slot.Primary
	Spec->DynamicGrantedTags.AddTag(CooldownTag);
	
	ApplyGameplayEffectSpecToOwner(Handle, ActorInfo, ActivationInfo, SpecHandle);
}

FGameplayTag UBaseGameplayAbility::GetCooldownTagFromInputID(const EAbilityInputID InputID) 
{
	switch (InputID)
	{
	case EAbilityInputID::Primary:
		return FGameplayTag::RequestGameplayTag(TEXT("Cooldown.Ability.Primary"));
	case EAbilityInputID::Secondary:
		return FGameplayTag::RequestGameplayTag(TEXT("Cooldown.Ability.Secondary"));
	case EAbilityInputID::Movement:
		return FGameplayTag::RequestGameplayTag(TEXT("Cooldown.Ability.Movement"));
	case EAbilityInputID::Utility:
		return FGameplayTag::RequestGameplayTag(TEXT("Cooldown.Ability.Utility"));
	case EAbilityInputID::Ultimate:
		return FGameplayTag::RequestGameplayTag(TEXT("Cooldown.Ability.Ultimate"));
	default:
		return FGameplayTag(); // empty tag
	}
}
