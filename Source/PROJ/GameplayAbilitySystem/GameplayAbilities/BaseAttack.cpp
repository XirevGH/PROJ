// Fill out your copyright notice in the Description page of Project Settings.

#include "BaseAttack.h"
#include "Abilities/Tasks/AbilityTask_WaitGameplayEvent.h"
#include "PROJ/Characters/BaseCharacter.h"
#include "PROJ/Data/AbilityData.h"

UBaseAttack::UBaseAttack()
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
	bIsHitscanActive = false;
	bHasRequestedHitScanStart = false;
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
	
	SetupHitScanTasks();
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
void UBaseAttack::StartHitScan()
{
	if (!HasAuthority(&CurrentActivationInfo) || bIsHitscanActive) return;
	
	bIsHitscanActive = true;
	Targets.Empty();
	
	PerformHitScan();
	
	if (!GetWorld()->GetTimerManager().IsTimerActive(HitScanTimerHandle))
	{
		GetWorld()->GetTimerManager().SetTimer(
			HitScanTimerHandle,
			this,
			&UBaseAttack::PerformHitScan,
			AbilityData->HitScanInterval,
			true);
	}
}

void UBaseAttack::EndHitScan()
{
	UE_LOG(LogTemp, Warning, TEXT("[UBaseAttack] EndHitScan called. bIsHitscanActive=%d"), (int)bIsHitscanActive);
	if (!bIsHitscanActive ||HasAuthority(&CurrentActivationInfo))
	{
		UE_LOG(LogTemp, Verbose, TEXT("[UBaseAttack] EndHitScan: not active"));
		return;
	}
	if (!CurrentActorInfo || !CurrentActorInfo->IsNetAuthority())
		return;
		
	bIsHitscanActive = false;
	
	Targets.Empty();
	
	if (CurrentActorInfo && CurrentActorInfo->IsNetAuthority())
	{
		GetWorld()->GetTimerManager().ClearTimer(HitScanTimerHandle);
		UE_LOG(LogTemp, Warning, TEXT("[UBaseAttack] SERVER: HitScan stopped"));
	}
}
void UBaseAttack::OnHitscanStart(FGameplayEventData Payload)
{
	if (!bIsHitscanActive)
	{
		if (CurrentActorInfo->IsNetAuthority())
		{
			StartHitScan();
		}
		else
		{
			Server_HitScanStart();
		}
	}
}

void UBaseAttack::OnHitscanEnd(FGameplayEventData Payload)
{
	UE_LOG(LogTemp, Warning, TEXT("[UBaseAttack] OnHitscanEnd received"));
	
	if (CurrentActorInfo && CurrentActorInfo->IsNetAuthority())
	{
		EndHitScan();
	}
}
void UBaseAttack::PerformHitScan()
{
	UE_LOG(LogTemp, Warning, TEXT("[UBaseAttack] PerformHitScan tick. IsNetAuthority=%d, bIsHitscanActive=%d"),
		(CurrentActorInfo ? CurrentActorInfo->IsNetAuthority() : 0),
		(int)bIsHitscanActive);
	
	//if (!CurrentActorInfo || !CurrentActorInfo->IsNetAuthority()) return;
	if (!bIsHitscanActive) return;
	
	FVector Start = GetSocketLocation(WeaponStartSocket);
	FVector End = GetSocketLocation(WeaponEndSocket);

	UE_LOG(LogTemp, Verbose, TEXT("[UBaseAttack] Trace owner: %s, Start:%s End:%s"),
		*GetAvatarActorFromActorInfo()->GetName(), *Start.ToString(), *End.ToString());

	FCollisionQueryParams TraceParams(FName(TEXT("WeaponTrace")), true, GetAvatarActorFromActorInfo());
	TraceParams.bReturnPhysicalMaterial = false;
    
	TArray<FHitResult> HitResults;
	if (GetWorld()->LineTraceMultiByChannel(HitResults, Start, End, ECC_Pawn, TraceParams))
	{
		for (auto& Hit : HitResults)
		{
			AActor* HitActor = Hit.GetActor();
			if (!HitActor || Targets.Contains(HitActor)) continue;

			Targets.Add(HitActor);
			
			if (AbilityData)
			{
				ApplyEffectsToTarget(HitActor);
			}
		}
	}

#if WITH_EDITOR
	DrawDebugLine(GetWorld(), Start, End, FColor::Green, false, 1.f, 0, 2.f);
#endif
}

void UBaseAttack::Server_HitScanStart_Implementation()
{	
	UE_LOG(LogTemp, Error, TEXT("SERVER_HitScanStart CALLED!"));
	StartHitScan();
}

void UBaseAttack::Server_EndHitScan_Implementation()
{
	EndHitScan();
}

FVector UBaseAttack::GetSocketLocation(const FName& SocketName) const
{
	if (ABaseCharacter* Player = Cast<ABaseCharacter>(GetAvatarActorFromActorInfo()))
	{
		if (USkeletalMeshComponent* Mesh = Player->GetMesh())
		{
			return Mesh->GetSocketLocation(SocketName);
		}
	}
	return FVector::ZeroVector;
}

void UBaseAttack::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicatedEndAbility, bool bWasCancelled)
{
	
		ClearExistingTasks();
		EndHitScan();
		bIsHitscanActive = false;
		bHasRequestedHitScanStart = false;
	
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicatedEndAbility, bWasCancelled);
}

