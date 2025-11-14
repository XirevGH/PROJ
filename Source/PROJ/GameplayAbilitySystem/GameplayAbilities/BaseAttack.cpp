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
	
	ABaseCharacter* Player = Cast<ABaseCharacter>(GetAvatarActorFromActorInfo());
	if (Player)
	{
		AWeapon* CurrentWeapon = Player->EquippedWeapon;
		if (!CurrentWeapon)
		{
			UE_LOG(LogTemp, Warning, TEXT("No equipped weapon found! Ability will end."));
			EndAbility(Handle, ActorInfo, ActivationInfo, true, false);
			return;
		}
		EquippedWeapon = CurrentWeapon;
		if (ActorInfo->OwnerActor->HasAuthority() || ActorInfo->IsLocallyControlled())
		{
			if (EquippedWeapon)
				EquippedWeapon->OnWeaponHit.AddDynamic(this, &UBaseAttack::OnWeaponHitReceived);
		}
	}
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
		
	if (ActorInfo->OwnerActor->HasAuthority())
	{
		// Wait for start
		auto* WaitEventTask = UAbilityTask_WaitGameplayEvent::WaitGameplayEvent(
			  this,
			  FGameplayTag::RequestGameplayTag(FName("Event.HitScan.Start")),
			  nullptr,
			  false,
			  false
		  );
		WaitEventTask->EventReceived.AddDynamic(this, &UBaseAttack::OnHitscanStart);
		WaitEventTask->ReadyForActivation();

		// Wait for end
		auto* WaitEndTask = UAbilityTask_WaitGameplayEvent::WaitGameplayEvent(
			this,
			FGameplayTag::RequestGameplayTag(FName("Event.HitScan.End")),
			nullptr,
			false,
			false
		);
		WaitEndTask->EventReceived.AddDynamic(this, &UBaseAttack::OnHitscanEnd);
		WaitEndTask->ReadyForActivation();
	}
}

void UBaseAttack::OnWeaponHitReceived(const TArray<FHitResult>& HitResults)
{
	for (auto& Hit : HitResults)
	{
		if (AActor* HitActor = Hit.GetActor())
		{
			if (!HitActors.Contains(HitActor))
			{
				HitActors.Add(HitActor);

				UE_LOG(LogTemp, Warning, TEXT("Ability hit actor: %s"), *HitActor->GetName());

				// Apply damage once
				ApplyDamage(HitActor);
			}
		}
	}
}

void UBaseAttack::ApplyDamage(AActor* Target)
{
	
}
void UBaseAttack::OnHitscanStart(FGameplayEventData Payload)
{
	if (EquippedWeapon)
	{
		EquippedWeapon->HitScanStart();
		UE_LOG(LogTemp, Warning, TEXT("Attack scan start"));
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Curren weapon null in HitScanStart"));
	}
}

void UBaseAttack::OnHitscanEnd(FGameplayEventData Payload)
{
	
	if (EquippedWeapon)
	{
		EquippedWeapon->HitScanEnd();
		UE_LOG(LogTemp, Warning, TEXT("Attack scan ended"));
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

void UBaseAttack::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicatedEndAbility, bool bWasCancelled)
{
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicatedEndAbility, bWasCancelled);
	
	if (EquippedWeapon)
	{
		EquippedWeapon->OnWeaponHit.RemoveDynamic(this, &UBaseAttack::OnWeaponHitReceived);
	}
	EquippedWeapon = nullptr;
	HitActors.Empty();
}
