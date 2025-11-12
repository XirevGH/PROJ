// Fill out your copyright notice in the Description page of Project Settings.

#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"
#include "Abilities/Tasks/AbilityTask_WaitGameplayEvent.h"
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
	ActorInfo->AvatarActor->GetComponentByClass()
	if (!MyMontage)
	{
		UE_LOG(LogTemp, Warning, TEXT("MyMontage is null"));
		EndAbility(Handle, ActorInfo, ActivationInfo, true, false);
		return;
	}
	if (ActorInfo && ActorInfo->OwnerActor.Get() && ActorInfo->OwnerActor->HasAuthority())
	{
		//Montage start
		auto* MontageTask = UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(
	this,
	TEXT("MyMontageTask"),
	MyMontage,
	1.0f,
	NAME_None,
	true);
	
		MontageTask->OnCompleted.AddDynamic(this, &UGA_Basic_Attack::OnMontageCompleted);
		MontageTask->OnInterrupted.AddDynamic(this, &UGA_Basic_Attack::OnMontageInterrupted);
		MontageTask->OnCancelled.AddDynamic(this, &UGA_Basic_Attack::OnMontageCancelled);

		MontageTask->ReadyForActivation();

		// Wait for start
		auto* WaitEventTask = UAbilityTask_WaitGameplayEvent::WaitGameplayEvent(
			  this,
			  FGameplayTag::RequestGameplayTag(FName("Event.HitScan.Start")), // Replace with your tag
			  nullptr,
			  false,
			  false
		  );

		WaitEventTask->EventReceived.AddDynamic(this, &UGA_Basic_Attack::OnHitscanStart);
		WaitEventTask->ReadyForActivation();

		// Wait for end
		auto* WaitEndTask = UAbilityTask_WaitGameplayEvent::WaitGameplayEvent(
			this,
			FGameplayTag::RequestGameplayTag(FName("Event.HitscanEnd")),
			nullptr,
			false,
			false
		);
		WaitEndTask->EventReceived.AddDynamic(this, &UGA_Basic_Attack::OnHitscanEnd);
		WaitEndTask->ReadyForActivation();
		
	}
	
}

void UGA_Basic_Attack::OnHitscanStart()
{
}

void UGA_Basic_Attack::OnHitscanEnd()
{
}


void UGA_Basic_Attack::Attack()
{
	UE_LOG(LogTemp, Warning, TEXT("Attack"));
}

void UGA_Basic_Attack::OnMontageCompleted()
{
	Attack();
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
}

void UGA_Basic_Attack::OnMontageInterrupted()
{
	UE_LOG(LogTemp, Warning, TEXT("Attack montage interrupted"));
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
}

void UGA_Basic_Attack::OnMontageCancelled()
{
	UE_LOG(LogTemp, Warning, TEXT("Attack montage cancelled"));
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
}
