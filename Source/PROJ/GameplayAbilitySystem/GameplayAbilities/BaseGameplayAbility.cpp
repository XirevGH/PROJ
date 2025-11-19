// Fill out your copyright notice in the Description page of Project Settings.


#include "BaseGameplayAbility.h"
#include "AbilitySystemComponent.h"
#include "AbilitySystemGlobals.h"
#include "PROJ/GameplayAbilitySystem/GameplayEffects/GE_BaseCooldown.h"



void UBaseGameplayAbility::OnGiveAbility(const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilitySpec& Spec)
{
	Super::OnGiveAbility(ActorInfo, Spec);

	FGameplayTag CooldownTag = GetCooldownTagFromInputID(InputTag);

	ActivationBlockedTags.AddTag(CooldownTag);
	
}

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
	//UE_LOG(LogTemp, Warning, TEXT("Apply Cooldown to %s "), *ActorInfo->AvatarActor->GetName());
	
	const FGameplayTag& CooldownTag = GetCooldownTagFromInputID(InputTag); // e.g., Cooldown.Slot.Primary
	
	Spec->DynamicGrantedTags.AddTag(CooldownTag);
	ApplyGameplayEffectSpecToOwner(Handle, ActorInfo, ActivationInfo, SpecHandle);
	//UE_LOG(LogTemp, Warning, TEXT("Apply Cooldown is valid: %s"),
	//	ApplyGameplayEffectSpecToOwner(Handle, ActorInfo, ActivationInfo, SpecHandle).IsValid() ? TEXT("True") :TEXT("False"));
}

TArray<FGameplayEffectSpecHandle> UBaseGameplayAbility::MakeEffectSpecsHandles(
	const TArray<TSubclassOf<UGameplayEffect>>& InEffects)
{
	TArray<FGameplayEffectSpecHandle> Specs;

	 UAbilitySystemComponent* CasterASC = GetAbilitySystemComponentFromActorInfo();
	if (!GetAbilitySystemComponentFromActorInfo()) return Specs;

	for (TSubclassOf<UGameplayEffect> EffectClass : InEffects)
	{
		if (!EffectClass) continue;

		FGameplayEffectContextHandle EffectContext = CasterASC->MakeEffectContext();
		FGameplayEffectSpecHandle SpecHandle = CasterASC->MakeOutgoingSpec(EffectClass, GetAbilityLevel(), EffectContext);

		if (!SpecHandle.IsValid()) continue;

		FGameplayEffectSpec* Spec = SpecHandle.Data.Get();

		// Apply runtime values for SetByCaller tags
		TArray<FGameplayTag> Tags;
		//Spec->GetAllSetByCallerTags(Tags);
		for (const FGameplayTag& Tag : Tags)
		{
			if (SetByCallerValues.Contains(Tag))
			{
				Spec->SetSetByCallerMagnitude(Tag, SetByCallerValues[Tag]);
			}
		}

		Specs.Add(SpecHandle);
	}

	return Specs;
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
