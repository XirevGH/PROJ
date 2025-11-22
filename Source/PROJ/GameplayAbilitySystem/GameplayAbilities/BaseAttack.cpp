// Fill out your copyright notice in the Description page of Project Settings.

#include "BaseAttack.h"
#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"
#include "Abilities/Tasks/AbilityTask_WaitGameplayEvent.h"
#include "PROJ/Characters/BaseCharacter.h"
#include "PROJ/Weapon/Weapon.h"

UBaseAttack::UBaseAttack()
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
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
	
	if (!MyMontage)
	{
		UE_LOG(LogTemp, Warning, TEXT("MyMontage is null"));
		EndAbility(Handle, ActorInfo, ActivationInfo, true, false);
		return;
	}
	PlayMontage();
	
	if (!HasAuthority(&ActivationInfo)) return;

	if (!SetupPlayerWeapon()) return;

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


void UBaseAttack::PlayMontage()
{
	/*Montage start*/
	auto* MontageTask = UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(
	this,
	TEXT("MyMontageTask"),
	MyMontage,
	1.0f,
	NAME_None,
	true);
	
	MontageTask->OnCompleted.AddDynamic(this, &UBaseAttack::OnMontageCompleted);
	MontageTask->OnInterrupted.AddDynamic(this, &UBaseAttack::OnMontageInterrupted);
	MontageTask->OnCancelled.AddDynamic(this, &UBaseAttack::OnMontageCancelled);

	MontageTask->ReadyForActivation();
}

void UBaseAttack::OnHitscanStart(FGameplayEventData Payload)
{
	if (!EquippedWeapon)
	{
		UE_LOG(LogTemp, Warning, TEXT("Current weapon null in HitScanStart"));
		return;
	}
	
	if (!EquippedWeapon->bIsHitscanActive)
	{
		if (!EquippedWeapon->HasAuthority())
			EquippedWeapon->Server_HitScanStart(1.f/30.f);
		else
			EquippedWeapon->HitScanStart(1.f/30.f);
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
void UBaseAttack::OnMontageCompleted()
{
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
}

void UBaseAttack::OnMontageInterrupted()
{
	UE_LOG(LogTemp, Warning, TEXT("Attack montage interrupted"));
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
}

void UBaseAttack::OnMontageCancelled()
{
	UE_LOG(LogTemp, Warning, TEXT("Attack montage cancelled"));
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
}

