// Fill out your copyright notice in the Description page of Project Settings.

#include "BaseAttack.h"
#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"
#include "Abilities/Tasks/AbilityTask_WaitGameplayEvent.h"
#include "PROJ/Characters/BaseCharacter.h"
#include "PROJ/Data/AbilityData.h"
#include "PROJ/Weapon/Weapon.h"

UBaseAttack::UBaseAttack()
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
}

void UBaseAttack::OnGiveAbility(const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilitySpec& Spec)
{
	Super::OnGiveAbility(ActorInfo, Spec);
}

void UBaseAttack::ActivateAbility(const FGameplayAbilitySpecHandle Handle,
                                  const FGameplayAbilityActorInfo* ActorInfo,
                                  const FGameplayAbilityActivationInfo ActivationInfo,
                                  const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);
	
	CurrentSpecHandle = Handle;
	CurrentActorInfo = ActorInfo;
	CurrentActivationInfo = ActivationInfo;
	
	if (!CommitAbility(Handle, ActorInfo, ActivationInfo))
	{
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		return;
	}
	if (!AbilityData)
	{
		UE_LOG(LogTemp, Error, TEXT("AttackData not set on ability BaseAttack"))
		EndAbility(Handle, ActorInfo, ActivationInfo, true, false);
		return;
	}
	PlayMontage(AbilityData->Montage);
	
	if (!HasAuthority(&ActivationInfo)) return;

	if (!SetupPlayerWeapon()) return;
	if (AbilityData->bUseHitScan)
		SetupHitScanTasks();
}
bool UBaseAttack::SetupPlayerWeapon()
{
	const ABaseCharacter* Player = Cast<ABaseCharacter>(GetAvatarActorFromActorInfo());
	if (!Player) return false;
		
	EquippedWeapon = Player->EquippedWeapon;
	if (!EquippedWeapon)
	{
		UE_LOG(LogTemp, Warning, TEXT("No equipped weapon found! Ability will end."));
		EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
		return false;
	}
	EquippedWeapon->Ability = this;
	return true;
}

void UBaseAttack::SetupHitScanTasks()
{
	ClearExistingTasks();

	/*Wait for start*/
	StartTask = UAbilityTask_WaitGameplayEvent::WaitGameplayEvent(
	this,
	FGameplayTag::RequestGameplayTag(FName("Event.HitScan.Start")),
	nullptr,
	true,
	false);
	
	StartTask->EventReceived.AddDynamic(this, &UBaseAttack::OnHitscanStart);
	StartTask->ReadyForActivation();

	/*Wait for end*/
	EndTask = UAbilityTask_WaitGameplayEvent::WaitGameplayEvent(
	this,
	FGameplayTag::RequestGameplayTag(FName("Event.HitScan.End")),
	nullptr,
	true,
	false);
	
	EndTask->EventReceived.AddDynamic(this, &UBaseAttack::OnHitscanEnd);
	EndTask->ReadyForActivation();
}

void UBaseAttack::ClearExistingTasks()
{
	if (StartTask) {StartTask->EndTask(); StartTask = nullptr;};
	if (EndTask) {EndTask->EndTask(); EndTask = nullptr;};
}

void UBaseAttack::OnHitscanStart(FGameplayEventData Payload)
{
	if (!EquippedWeapon) return;

	float Interval = AbilityData->HitScanInterval;
	
	if (!EquippedWeapon->bIsHitscanActive)
	{
		if (!EquippedWeapon->HasAuthority())
			EquippedWeapon->Server_HitScanStart(Interval);
		else
			EquippedWeapon->HitScanStart(Interval);
	}
}

void UBaseAttack::OnHitscanEnd(FGameplayEventData Payload)
{
	if (!HasAuthority(&CurrentActivationInfo)) return;
	
	if (EquippedWeapon && bIsHitscanActive)
	{
		EquippedWeapon->HitScanEnd();
		bIsHitscanActive = false;
	}
}
void UBaseAttack::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicatedEndAbility, bool bWasCancelled)
{
	if (HasAuthority(&ActivationInfo))
	{
		ClearExistingTasks();
		
		if (EquippedWeapon)
		{
			EquippedWeapon->HitScanEnd();
			bIsHitscanActive = false;
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("EndAbility called but weapon is null!"));
		}
	}
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicatedEndAbility, bWasCancelled);
}

