// Fill out your copyright notice in the Description page of Project Settings.


#include "GP_Dash.h"

#include "AbilitySystemComponent.h"
#include "GameplayTagContainer.h"
#include "GameplayEffect.h"
#include "GameFramework/Character.h"
#include "GEC_BaseCooldown.h"

UGP_Dash::UGP_Dash()
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
	NetExecutionPolicy = EGameplayAbilityNetExecutionPolicy::LocalPredicted;

	// Define the tags used by this ability
	DashTag = FGameplayTag::RequestGameplayTag(FName("Ability.Movement.Dash"));
	StunnedTag = FGameplayTag::RequestGameplayTag(FName("State.Stunned"));
	CooldownTag = FGameplayTag::RequestGameplayTag(FName("Cooldown.Dash"));

	CooldownGameplayEffect = UGEC_BaseCooldown::StaticClass();

	//Block activation om actor har stun tag.
	BlockAbilitiesWithTag.AddTag(StunnedTag);
}

void UGP_Dash::ActivateAbility(
	const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo,
	const FGameplayEventData* TriggerEventData)
{
	if (!CommitAbility(Handle, ActorInfo, ActivationInfo))
	{
		EndAbility(Handle, ActorInfo, ActivationInfo, true, false);
		return;
	}
	
	// Check if actor is stunned
	if (ActorInfo->AbilitySystemComponent->HasMatchingGameplayTag(StunnedTag))
	{
		EndAbility(Handle, ActorInfo, ActivationInfo, true, false);
		return;
	}
	
	// Check for cooldown
	if (ActorInfo->AbilitySystemComponent->HasMatchingGameplayTag(CooldownTag))
	{
		
		EndAbility(Handle, ActorInfo, ActivationInfo, true, false);
		return;
	}
	// Dash logic
	ACharacter* Character = Cast<ACharacter>(ActorInfo->AvatarActor.Get());
	if (Character)
	{
		FVector DashDirection = Character->GetActorForwardVector();
		Character->LaunchCharacter(DashDirection * 1500, true, true);
	}

	// Apply cooldown effect
	if (CooldownGameplayEffect)
	{
		FGameplayEffectSpecHandle SpecHandle = MakeOutgoingGameplayEffectSpec(CooldownGameplayEffect, GetAbilityLevel());
		if (SpecHandle.IsValid())
		{
			SpecHandle.Data->SetDuration(CooldownDuration,true);
			SpecHandle.Data->DynamicGrantedTags.AddTag(CooldownTag);
			FActiveGameplayEffectHandle HandleApplied = ActorInfo->AbilitySystemComponent->ApplyGameplayEffectSpecToSelf(*SpecHandle.Data.Get());
			
			if (HandleApplied.IsValid())
			{
				UE_LOG(LogTemp, Warning, TEXT("Cooldown applied successfully!"));
			}
		}
	}
	EndAbility(Handle, ActorInfo, ActivationInfo, false, false);
}