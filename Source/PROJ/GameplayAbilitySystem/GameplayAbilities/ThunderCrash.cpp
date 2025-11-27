// Fill out your copyright notice in the Description page of Project Settings.


#include "ThunderCrash.h"

#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"
#include "Abilities/Tasks/AbilityTask_WaitGameplayEvent.h"
#include "PROJ/AbilityActors/AbilityActor.h"
#include "PROJ/Characters/BaseCharacter.h"
#include "PROJ/Data/AttackData.h"

UThunderCrash::UThunderCrash()
{ 
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
}

void UThunderCrash::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
                                    const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	if (!CommitAbility(Handle, ActorInfo, ActivationInfo))
	{
		EndAbility(Handle, ActorInfo, ActivationInfo, true, false);
	}
	CachedPlayer = Cast<ABaseCharacter>(GetAvatarActorFromActorInfo());
	if (!CachedPlayer) return;
	
	
	//PlayMontage(AttackData->Montage);
	//MakeMontageWaitEvent();
	SpawnConduit();
	EndAbility(Handle, ActorInfo, ActivationInfo, true, false);
}
void UThunderCrash::OnMontageNotifyReceived(FGameplayEventData Payload)
{
	SpawnConduit();
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
}

void UThunderCrash::SpawnConduit()
{
	FActorSpawnParameters Params;
	Params.Owner = CachedPlayer;
	Params.Instigator = CachedPlayer;

	AAbilityActor* SpawnedActor = GetWorld()->SpawnActor<AAbilityActor>(
		ConduitActor,
		CachedPlayer->GetActorLocation(),
		CachedPlayer->GetActorRotation(),Params);
	if (SpawnedActor)
	{
		SpawnedActor->InitializeAbilityActor(
			CachedPlayer,
			CachedPlayer->GetAbilitySystemComponent(),
			this,
			MakeEffectSpecsHandles());
	}
}

void UThunderCrash::PlayMontage(UAnimMontage* Montage)
{
	Super::PlayMontage(Montage);
}

void UThunderCrash::MakeMontageWaitEvent()
{
	if (MontageNotifyTag.IsValid())
	{
		UAbilityTask_WaitGameplayEvent* Task = UAbilityTask_WaitGameplayEvent::WaitGameplayEvent(
			this,
			MontageNotifyTag,
			nullptr,
			true,
			true);

		Task->EventReceived.AddDynamic(this, &UThunderCrash::OnMontageNotifyReceived);
		Task->ReadyForActivation();
	}
}