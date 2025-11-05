// Fill out your copyright notice in the Description page of Project Settings.


#include "GP_Dash.h"
#include "GameFramework/Character.h"

UGP_Dash::UGP_Dash()
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
}

void UGP_Dash::ActivateAbility(
	const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo,
	const FGameplayEventData* TriggerEventData)
{
	UE_LOG(LogTemp, Display, TEXT("Activating Dash Ability CPP"));
	if (!CommitAbility(Handle, ActorInfo, ActivationInfo))
	{
		EndAbility(Handle, ActorInfo, ActivationInfo, true, false);
		return;
	}

	ACharacter* Character = Cast<ACharacter>(ActorInfo->AvatarActor.Get());
	if (Character)
	{
		FVector DashDirection = Character->GetActorForwardVector();
		Character->LaunchCharacter(DashDirection * 2500, true, true);
	}

	EndAbility(Handle, ActorInfo, ActivationInfo, false, false);
}