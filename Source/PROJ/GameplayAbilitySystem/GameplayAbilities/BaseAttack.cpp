// Fill out your copyright notice in the Description page of Project Settings.

#include "BaseAttack.h"
#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"
#include "Abilities/Tasks/AbilityTask_WaitGameplayEvent.h"
#include "PROJ/BaseCharacter.h"
#include "PROJ/Weapon/Weapon.h"

UBaseAttack::UBaseAttack()
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
}

void UBaseAttack::ActivateAbility(const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
	const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);
	
	if (!MyMontage)
	{
		UE_LOG(LogTemp, Warning, TEXT("MyMontage is null"));
		EndAbility(Handle, ActorInfo, ActivationInfo, true, false);
		return;
	}
	
	//Montage start
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
	if (HasAuthority(&ActivationInfo))
	{
		if (!CommitAbility(Handle, ActorInfo, ActivationInfo))
		{
			EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
			return;
		}
	
		ABaseCharacter* Player = Cast<ABaseCharacter>(GetAvatarActorFromActorInfo());
		if (Player)
		{
			EquippedWeapon = Player->EquippedWeapon;
			EquippedWeapon->Ability = this;
	
			if (!EquippedWeapon)
			{
				UE_LOG(LogTemp, Warning, TEXT("No equipped weapon found! Ability will end."));
				EndAbility(Handle, ActorInfo, ActivationInfo, true, false);
				return;
			}
		}
		
		
		if (ActorInfo && ActorInfo->OwnerActor.Get() && ActorInfo->OwnerActor->HasAuthority())
		{
			if (StartTask)
			{
				StartTask->EndTask();
				StartTask = nullptr;
			}

			if (EndTask)
			{
				EndTask->EndTask();
				EndTask = nullptr;
			}
		
			// Wait for start
			StartTask = UAbilityTask_WaitGameplayEvent::WaitGameplayEvent(
				  this,
				  FGameplayTag::RequestGameplayTag(FName("Event.HitScan.Start")),
				  nullptr,
				  true,
				  false
			  );
		
			StartTask->EventReceived.AddDynamic(this, &UBaseAttack::OnHitscanStart);
			StartTask->ReadyForActivation();

			// Wait for end
			EndTask = UAbilityTask_WaitGameplayEvent::WaitGameplayEvent(
				this,
				FGameplayTag::RequestGameplayTag(FName("Event.HitScan.End")),
				nullptr,
				true,
				false
			);
			EndTask->EventReceived.AddDynamic(this, &UBaseAttack::OnHitscanEnd);
			EndTask->ReadyForActivation();
		
		}
	}
}

void UBaseAttack::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicatedEndAbility, bool bWasCancelled)
{
	if (HasAuthority(&ActivationInfo))
	{
		if (StartTask)
			StartTask->EndTask();
		if (EndTask)
			EndTask->EndTask();
	
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

void UBaseAttack::OnHitscanStart(FGameplayEventData Payload)
{
	if (EquippedWeapon && !EquippedWeapon->bIsHitscanActive)
	{
		//bIsHitscanActive = true;

		if (!EquippedWeapon->HasAuthority())
		{
			EquippedWeapon->Server_HitScanStart(1.f/30.f);
		}
		else
		{
			EquippedWeapon->HitScanStart(1.f/30.f);
		}
	}
	else
		{
			bool bHasAuthority = HasAuthority(&CurrentActivationInfo);
			UE_LOG(LogTemp, Warning, TEXT("Curren weapon null in HitScanStart: %s, %hs,%s"),
				*EquippedWeapon->GetOwner()->GetName(),
				bIsHitscanActive ? "true" : "false",
				bHasAuthority ? TEXT("true") : TEXT("false"));
		}
	UE_LOG(LogTemp, Warning, TEXT("OnHitscanStart called. Authority: %s, bIsHitscanActive: %s, EquippedWeapon: %s"),
	HasAuthority(&CurrentActivationInfo) ? TEXT("SERVER") : TEXT("CLIENT"),
	bIsHitscanActive ? TEXT("true") : TEXT("false"),
	EquippedWeapon ? *EquippedWeapon->GetName() : TEXT("null"));
}

void UBaseAttack::OnHitscanEnd(FGameplayEventData Payload)
{
	if (!HasAuthority(&CurrentActivationInfo)) return;
	
	if (EquippedWeapon && bIsHitscanActive)
	{
		EquippedWeapon->HitScanEnd();
		bIsHitscanActive = false;
		UE_LOG(LogTemp, Warning, TEXT("Attack scan end"));
	}
	else
	{
	UE_LOG(LogTemp, Warning, TEXT("Curren weapon null in HitScanEnd"));
	}
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

