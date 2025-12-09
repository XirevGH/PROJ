// Fill out your copyright notice in the Description page of Project Settings.


#include "TempestOverload.h"
#include "PROJ/Characters/BaseCharacter.h"

void UTempestOverload::ActivateAbility(const FGameplayAbilitySpecHandle Handle,
                                       const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
                                       const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	const ABaseCharacter* Player = Cast<ABaseCharacter>(ActorInfo->AvatarActor.Get());
	if (!Player) return;
	
	UAbilitySystemComponent* ASC = Player->GetAbilitySystemComponent();
	if (!ASC) return;
	
	if (!CanActivateAbility(Handle,ActorInfo)) return;

	float Charges = ASC->GetNumericAttribute(UCharacterAttributeSet::GetConduitChargesAttribute());

	UE_LOG(LogTemp, Warning, TEXT("Conduit Charges = %f"), Charges);
	
	if (Charges <= 0) return;

	/*Consume charges*/
	ASC->ApplyModToAttribute(
		UCharacterAttributeSet::GetConduitChargesAttribute(),
		EGameplayModOp::Additive,
		-Charges);
	
	/*Calculate damage multiplier*/
	float BuffValue = Charges * DamageMultiplier;

	/*Apply gameplay effect*/
	FGameplayEffectSpecHandle SpecHandle = ASC->MakeOutgoingSpec(DamageBuffEffectClass,1,ASC->MakeEffectContext());
	if (!SpecHandle.IsValid()) return;

	SpecHandle.Data->SetSetByCallerMagnitude(
		FGameplayTag::RequestGameplayTag("Data.Damage.Buff"),
		BuffValue);

	ASC->ApplyGameplayEffectSpecToSelf(*SpecHandle.Data.Get());
	
	EndAbility(Handle,ActorInfo,ActivationInfo,true,false);
}

bool UTempestOverload::CanActivateAbility(const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo, const FGameplayTagContainer* SourceTags,
	const FGameplayTagContainer* TargetTags, FGameplayTagContainer* OptionalRelevantTags) const
{
	
	const ABaseCharacter* Player = Cast<ABaseCharacter>(ActorInfo->AvatarActor.Get());
	if (!Player) return false;

	const UAbilitySystemComponent* ASC = Player->GetAbilitySystemComponent();
	if (!ASC) return false;
	
	float Charges = ASC->GetNumericAttribute(UCharacterAttributeSet::GetConduitChargesAttribute());

	UE_LOG(LogTemp, Warning, TEXT("Conduit Charges = %f"), Charges);
	return Charges > 0;
}
