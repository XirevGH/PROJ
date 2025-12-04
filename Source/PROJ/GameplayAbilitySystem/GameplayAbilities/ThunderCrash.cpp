// Fill out your copyright notice in the Description page of Project Settings.


#include "ThunderCrash.h"

#include "AbilitySystemGlobals.h"
#include "Abilities/Tasks/AbilityTask_WaitGameplayEvent.h"
#include "Engine/OverlapResult.h"
#include "GameFramework/CharacterMovementComponent.h"
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
	if (!CanActivateAbility(Handle, ActorInfo)) return;
	
	if (!CommitAbility(Handle, ActorInfo, ActivationInfo))
	{
		EndAbility(Handle, ActorInfo, ActivationInfo, true, false);
	}
	CachedPlayer = Cast<ABaseCharacter>(GetAvatarActorFromActorInfo());
	if (!CachedPlayer) return;

	PlayMontage(AttackData->Montage);
	
	if (!HasAuthority(&ActivationInfo)) return;
	MakeMontageWaitEvent();
}

bool UThunderCrash::CanActivateAbility(const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo, const FGameplayTagContainer* SourceTags,
	const FGameplayTagContainer* TargetTags, FGameplayTagContainer* OptionalRelevantTags) const
{
	if (!Super::CanActivateAbility(Handle, ActorInfo, SourceTags, TargetTags, OptionalRelevantTags))
		return false;

	const ABaseCharacter* Player = Cast<ABaseCharacter>(GetAvatarActorFromActorInfo());
	if (!Player) return false;

	const UCharacterMovementComponent* MoveComp = Player->GetCharacterMovement();
	if (!MoveComp || !MoveComp->IsMovingOnGround()) return false;

	return true;
}

void UThunderCrash::OnMontageNotifyReceived(FGameplayEventData Payload)
{
	FGameplayTag TriggeredTag = Payload.EventTag;
	if (TriggeredTag.MatchesTagExact(FGameplayTag::RequestGameplayTag(TEXT("Ability.Attack.Melee"))))
	{
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Yellow, TEXT("Montage Notify Triggered!"));
		SpawnConduit();
	}
}

void UThunderCrash::SpawnConduit()
{
	FHitResult Hit;
	FVector Start = CachedPlayer->GetActorLocation();
	FVector End = Start - FVector(0.f, 0.f, 1000.f);
	if (GetWorld()->LineTraceSingleByChannel(Hit,Start,End,ECC_Visibility))
	{
		FVector SpawnLocation = Hit.ImpactPoint;
		FActorSpawnParameters Params;
		Params.Owner = CachedPlayer;
		Params.Instigator = CachedPlayer;

		AAbilityActor* SpawnedActor = GetWorld()->SpawnActor<AAbilityActor>(
			ConduitActor,
			SpawnLocation,
			CachedPlayer->GetActorRotation(),Params);
		
		if (SpawnedActor)
		{
			InitializeAbilityActor(SpawnedActor);
			SpawnedActor->SetLifeSpan(ConduitLifeTime);
		}
	}
}

void UThunderCrash::PlayMontage(UAnimMontage* Montage)
{
	if (CachedPlayer)
		CachedPlayer->bMovementInputBlocked = true;
	
	Super::PlayMontage(Montage);
}

void UThunderCrash::OnMontageCompleted()
{
	if (CachedPlayer)
		CachedPlayer->bMovementInputBlocked = false;
	
	Super::OnMontageCompleted();
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
