// Fill out your copyright notice in the Description page of Project Settings.


#include "BaseGameplayAbility.h"
#include "AbilitySystemComponent.h"
#include "AbilitySystemGlobals.h"
#include "PROJ/Data/AttackData.h"
#include "PROJ/AbilityActors/AbilityActor.h"

void UBaseGameplayAbility::OnGiveAbility(const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilitySpec& Spec)
{
	Super::OnGiveAbility(ActorInfo, Spec);

	if (AttackData && AttackData->bHasDamage)
	{
		ActivationBlockedTags.AppendTags(AttackData->GrantedTags);

		SetByCallerValues.Add(
			FGameplayTag::RequestGameplayTag("Data.Damage"),
			AttackData->Damage
		);
	}
	if (AttackData && AttackData->bHasSlow && AttackData->SlowEffect)
	{
		SetByCallerValues.Add(
			FGameplayTag::RequestGameplayTag("Data.Movement.Slow.Multiplier"),
			AttackData->SlowAmount
		);

		SetByCallerValues.Add(
			FGameplayTag::RequestGameplayTag("Data.Movement.Slow.Duration"),
			AttackData->SlowDuration
		);
	}
	if (AttackData && AttackData->bHasStun)
	{
		
		SetByCallerValues.Add(
			FGameplayTag::RequestGameplayTag("Data.Movement.Stun.Duration"),
			AttackData->StunDuration
		);
	}

	CooldownTag = GetCooldownTagFromInputID(InputTag);
	if (CooldownTag.IsValid())
	{
		CooldownTagContainer.AddTag(CooldownTag);
		ActivationBlockedTags.AddTag(CooldownTag);
	}
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
	
	//const FGameplayTag& CooldownTag = GetCooldownTagFromInputID(InputTag); // e.g., Cooldown.Slot.Primary
	
	Spec->DynamicGrantedTags.AddTag(CooldownTag);
	ApplyGameplayEffectSpecToOwner(Handle, ActorInfo, ActivationInfo, SpecHandle);

}

TArray<FGameplayEffectSpecHandle> UBaseGameplayAbility::MakeEffectSpecsHandles()
{
	TArray<FGameplayEffectSpecHandle> Specs;

	UAbilitySystemComponent* CasterASC = GetAbilitySystemComponentFromActorInfo();
	if (!CasterASC) return Specs;

	/*Damage*/
	if (AttackData && AttackData->bHasDamage && AttackData->DamageEffect)
	{
		FGameplayEffectContextHandle Context = CasterASC->MakeEffectContext();
		FGameplayEffectSpecHandle SpecHandle =  CasterASC->MakeOutgoingSpec(AttackData->DamageEffect,GetAbilityLevel(),Context);
		if (SpecHandle.IsValid())
		{
			SpecHandle.Data->SetSetByCallerMagnitude(FGameplayTag::RequestGameplayTag("Data.Damage"), AttackData->Damage);
			Specs.Add(SpecHandle);
		}
	}
	/*Slow*/
	if (AttackData && AttackData->bHasSlow && AttackData->SlowEffect)
	{
		FGameplayEffectContextHandle Context = CasterASC->MakeEffectContext();
		FGameplayEffectSpecHandle SpecHandle = CasterASC->MakeOutgoingSpec(AttackData->SlowEffect, GetAbilityLevel(), Context);
		if (SpecHandle.IsValid())
		{
			SpecHandle.Data->SetSetByCallerMagnitude(FGameplayTag::RequestGameplayTag("Data.Movement.Slow.Multiplier"), AttackData->SlowAmount);
			SpecHandle.Data->SetSetByCallerMagnitude(FGameplayTag::RequestGameplayTag("Data.Movement.Slow.Duration"), AttackData->SlowDuration);
			Specs.Add(SpecHandle);
		}
	}
	/*Stun*/
	if (AttackData && AttackData->bHasStun && AttackData->StunEffect)
	{
		FGameplayEffectContextHandle Context = CasterASC->MakeEffectContext();
		FGameplayEffectSpecHandle SpecHandle = CasterASC->MakeOutgoingSpec(AttackData->StunEffect, GetAbilityLevel(), Context);
		if (SpecHandle.IsValid())
		{
			SpecHandle.Data->SetSetByCallerMagnitude(FGameplayTag::RequestGameplayTag("Data.Movement.Stun.Duration"), AttackData->StunDuration);
			Specs.Add(SpecHandle);
		}
	}
	/*for (TSubclassOf<UGameplayEffect> EffectClass : AttackData->Effects)
	{
		if (!EffectClass) continue;

		FGameplayEffectContextHandle EffectContext = CasterASC->MakeEffectContext();
		FGameplayEffectSpecHandle SpecHandle = CasterASC->MakeOutgoingSpec(EffectClass, GetAbilityLevel(), EffectContext);

		if (!SpecHandle.IsValid()) continue;

		FGameplayEffectSpec* Spec = SpecHandle.Data.Get();
		if (!Spec) continue;

		// Apply values from member variable
		for (const TPair<FGameplayTag, float>& Pair : SetByCallerValues)
		{
			Spec->SetSetByCallerMagnitude(Pair.Key, Pair.Value);
		}
		
		Specs.Add(SpecHandle);
	}*/

	return Specs;
}

void UBaseGameplayAbility::InitializeAbilityActor(AAbilityActor* Actor)
{
	if (!Actor)
		return;
	
	Actor->SetReplicates(true);	
	Actor->SetReplicateMovement(true);
	Actor->InitializeAbilityActor(GetAvatarActorFromActorInfo(),
			GetAbilitySystemComponentFromActorInfo(),
			this,
			MakeEffectSpecsHandles()
			);
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

const FGameplayTagContainer* UBaseGameplayAbility::GetCooldownTags() const
{
	return &CooldownTagContainer;
}

void UBaseGameplayAbility::ApplyEffectsToTarget(AActor* Target)
{
	UAbilitySystemComponent* TargetASC =
		UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(Target);

	UAbilitySystemComponent* OwnerASC =
		GetAbilitySystemComponentFromActorInfo();

	if (!TargetASC || !OwnerASC)
		return;

	if (!OwnerASC->GetOwner()->HasAuthority())
		return;

	TArray<FGameplayEffectSpecHandle> Specs = MakeEffectSpecsHandles();

	for (const FGameplayEffectSpecHandle& Spec : Specs)
	{
		if (Spec.IsValid())
		{
			TargetASC->ApplyGameplayEffectSpecToSelf(*Spec.Data.Get());
		}
	}
}
