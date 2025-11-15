// Fill out your copyright notice in the Description page of Project Settings.


#include "GA_Basic_Attack.h"

UGA_Basic_Attack::UGA_Basic_Attack()
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
}

void UGA_Basic_Attack::ActivateAbility(const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
	const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);
	Attack();
	EndAbility(Handle, ActorInfo, ActivationInfo, true, false);
}

void UGA_Basic_Attack::Attack()
{
	UE_LOG(LogTemp, Warning, TEXT("Attack"));
	
}
