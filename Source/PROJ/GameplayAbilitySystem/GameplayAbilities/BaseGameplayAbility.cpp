// Fill out your copyright notice in the Description page of Project Settings.


#include "BaseGameplayAbility.h"
#include "AbilitySystemComponent.h"
#include "AbilitySystemGlobals.h"
#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"
#include "PROJ/Data/AttackData.h"
#include "PROJ/AbilityActors/AbilityActor.h"

void UBaseGameplayAbility::OnGiveAbility(const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilitySpec& Spec)
{
	Super::OnGiveAbility(ActorInfo, Spec);

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

void UBaseGameplayAbility::PlayMontage(UAnimMontage* Montage)
{
	UE_LOG(LogTemp, Display, TEXT("Playing Montage"));
	/*Montage start*/
	auto* MontageTask = UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(
	this,
	TEXT("MyMontageTask"),
	Montage,
	1.0f,
	NAME_None,
	true,
	1);

	
	MontageTask->OnCompleted.AddDynamic(this, &UBaseGameplayAbility::OnMontageCompleted);
	MontageTask->OnInterrupted.AddDynamic(this, &UBaseGameplayAbility::OnMontageInterrupted);
	MontageTask->OnCancelled.AddDynamic(this, &UBaseGameplayAbility::OnMontageCancelled);
	
	MontageTask->ReadyForActivation();
}

void UBaseGameplayAbility::OnMontageCompleted()
{
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
}

void UBaseGameplayAbility::OnMontageInterrupted()
{
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
}

void UBaseGameplayAbility::OnMontageCancelled()
{
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
}

TArray<FGameplayEffectSpecHandle> UBaseGameplayAbility::MakeEffectSpecsHandles()
{
	TArray<FGameplayEffectSpecHandle> Specs;

	UAbilitySystemComponent* CasterASC = GetAbilitySystemComponentFromActorInfo();
	if (!CasterASC)
		return Specs;

	const TArray<FAttackEffectEntry>& EffectsToUse = (AttackData && AttackData->Effects.Num() > 0) ? AttackData->Effects : Effects;
	FGameplayEffectContextHandle Context = CasterASC->MakeEffectContext();
	
	for (const FAttackEffectEntry& Entry : EffectsToUse)
	{
		if (!Entry.Effect) continue;

		FGameplayEffectSpecHandle Spec = CasterASC->MakeOutgoingSpec(Entry.Effect, GetAbilityLevel(), Context);

		if (!Spec.IsValid()) continue;

		for (const auto& Pair : Entry.SetByCallerValues)
		{
			Spec.Data->SetSetByCallerMagnitude(Pair.Key, Pair.Value);
		}

		Specs.Add(Spec);
	}

	return Specs;
}

void UBaseGameplayAbility::InitializeAbilityActor(AAbilityActor* Actor)
{
	if (!Actor)
		return;
	
	TArray<FGameplayEffectSpecHandle> Specs = MakeEffectSpecsHandles();
	Actor->SetReplicates(true);	
	Actor->SetReplicateMovement(true);
	Actor->InitializeAbilityActor(
		GetAvatarActorFromActorInfo(),
		GetAbilitySystemComponentFromActorInfo(),
		this,
		Specs
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

