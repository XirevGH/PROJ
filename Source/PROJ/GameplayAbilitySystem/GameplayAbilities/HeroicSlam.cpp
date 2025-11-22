// Fill out your copyright notice in the Description page of Project Settings.


#include "HeroicSlam.h"

#include "AbilitySystemGlobals.h"
#include "Abilities/Tasks/AbilityTask_WaitTargetData.h"
#include "Components/CapsuleComponent.h"
#include "Engine/OverlapResult.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "PROJ/BaseCharacter.h"



UHeroicSlam::UHeroicSlam()
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
}

void UHeroicSlam::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
                                  const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	if (!CommitAbility(Handle, ActorInfo, ActivationInfo))
	{
		EndAbility(Handle, ActorInfo, ActivationInfo, true, false);
	}
	/*Get and Check CachedPlayer*/
	CachedPlayer = Cast<ABaseCharacter>(ActorInfo->AvatarActor.Get());
	if (!CachedPlayer) return;
	
	/*Shoot up*/
	FVector Direction = CachedPlayer->GetActorUpVector();
	CachedPlayer->LaunchCharacter(Direction * 1000.f, true,true);
	
	/*Null Check*/
	if (!IndicatorClass) return;
	
	/*Task & delegate setup*/
	UAbilityTask_WaitTargetData* Task = UAbilityTask_WaitTargetData::WaitTargetData(
	this,
	FName("WaitForTarget"),
	EGameplayTargetingConfirmation::UserConfirmed,
	IndicatorClass);
	/***/
	Task->ValidData.AddDynamic(this, &UHeroicSlam::OnConfirm);
	Task->Cancelled.AddDynamic(this, &UHeroicSlam::OnCancel);
	UE_LOG(LogTemp, Warning, TEXT("Task %s"), Task->IsValidLowLevel() ? TEXT("Successfully activated") : TEXT("Failed to activate"));
	Task->ReadyForActivation();
	
}

void UHeroicSlam::OnConfirm(const FGameplayAbilityTargetDataHandle& Data)
{
	/*Get Location Marked With Indicator*/
	if (Data.Num() > 0)
	{
		/*Gets TargetData*/
		const FGameplayAbilityTargetData* TargetData = Data.Get(0);
		if (TargetData)
		{
			/*Sets TargetLocation via HitData*/
			if (const FGameplayAbilityTargetData_SingleTargetHit* HitData =
				static_cast<const FGameplayAbilityTargetData_SingleTargetHit*>(TargetData))
			{
				TargetLocation = HitData->HitResult.ImpactPoint;
			}
			/*Sets TargetLocation via LocationData if no there is no HitData*/
			else if (const FGameplayAbilityTargetData_LocationInfo* LocationData =
				static_cast<const FGameplayAbilityTargetData_LocationInfo*>(TargetData))
			{
				TargetLocation = LocationData->GetEndPoint();
			}
			/*Call Launch Logic*/
			LaunchToTarget();
		}
	}
}

void UHeroicSlam::RestorAirFriction()
{
	auto* Move = CachedPlayer->GetCharacterMovement();
	Move->MaxWalkSpeed = CachedOriginalMaxSpeed;
	Move->AirControl = OriginalAirControl;
	Move->BrakingDecelerationFalling = OriginalBraking;
	Move->FallingLateralFriction = OriginalFriction;
	Move->SetMovementMode(MOVE_Walking);
}

void UHeroicSlam::LaunchToTarget()
{
	ESuggestProjVelocityTraceOption::Type TraceOption = ESuggestProjVelocityTraceOption::OnlyTraceWhileAscending;
	FCollisionResponseParams& ResponseParam = FCollisionResponseParams::DefaultResponseParam;
	TArray<AActor*> ActorsToIgnore;
	ActorsToIgnore.Add(CachedPlayer);
	
	FVector Start = CachedPlayer->GetActorLocation();
	FVector End = TargetLocation;
	/*Because Target location is on ground level and player start is above ground(Better calculation imo*TEST*)*/
	End.Z += CachedPlayer->GetCapsuleComponent()->GetScaledCapsuleHalfHeight();
	
	FVector LaunchVelocity;
	bool bHasSolution = UGameplayStatics::SuggestProjectileVelocity(
		this,
		LaunchVelocity,
		Start,
		End,
		1800.f,
		false,
		0.f,
		0.f,
		TraceOption,
		ResponseParam,
		ActorsToIgnore,
		true,
		false);
	
	if (!bHasSolution) return;

	auto* Move = CachedPlayer->GetCharacterMovement();
	if (!Move) return;

	/*Save original settings to restore later*/
	CachedOriginalMaxSpeed = Move->MaxWalkSpeed;
	OriginalAirControl = Move->AirControl;
	OriginalBraking = Move->BrakingDecelerationFalling;
	OriginalFriction = Move->FallingLateralFriction;

	/*Disable stuff that reduce arc distance*/
	Move->AirControl = 0.f;
	Move->BrakingDecelerationFalling = 0.f;
	Move->FallingLateralFriction = 0.f;

	/*Increase MaxWalkSpeed*/
	Move->MaxWalkSpeed = LaunchVelocity.Size();
	/*Movementmodes*/
	Move->StopMovementImmediately();
	Move->SetMovementMode(MOVE_Falling);
	
	/*Launch player in an arc*/
	CachedPlayer->LaunchCharacter(LaunchVelocity, true, true);

	/*Set a timer to check every 0.1 sec to see if the player landed to restore MS*/
	GetWorld()->GetTimerManager().SetTimer(
	LandingCheckTimer,
	this,
	&UHeroicSlam::LandingCheck,
	0.1f,
	true);
}
void UHeroicSlam::LandingCheck()
{
	if (!CachedPlayer->HasAuthority()) return;
	if (!CachedPlayer) return;
	if (!CachedPlayer->GetCharacterMovement()->IsMovingOnGround()) return;

	auto* Move = CachedPlayer->GetCharacterMovement();
	FVector Vel = Move->Velocity;
	Vel.X = 0.f,
	Vel.Y = 0.f;
	Move->Velocity = Vel;
	
	FVector Origin = CachedPlayer->GetActorLocation();
	TArray<FOverlapResult> Overlaps;

	FCollisionQueryParams Params;
	Params.AddIgnoredActor(CachedPlayer);

	bool bHit = GetWorld()->OverlapMultiByChannel(
		Overlaps,
		Origin,
		FQuat::Identity,
		ECC_Pawn,
		FCollisionShape::MakeSphere(SlamRadius),
		Params);

	if (bHit)
	{
		for (auto& Result : Overlaps)
		{
			AActor* HitActor = Result.GetActor();
			if (!HitActor || HitActor == CachedPlayer) continue;

			ApplyEffectsToTarget(HitActor);
		}
	}
	DrawDebugSphere(GetWorld(), Origin, SlamRadius, 32, FColor::Blue, false, 2.f);
	/*Reset all movement attributes*/
	RestorAirFriction();
	/*Reset timer*/
	GetWorld()->GetTimerManager().ClearTimer(LandingCheckTimer);
	
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
	
}
void UHeroicSlam::OnCancel(const FGameplayAbilityTargetDataHandle& Data)
{
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
}

void UHeroicSlam::ApplyEffectsToTarget(AActor* Target)
{
	UAbilitySystemComponent* TargetASC
	= UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(Target);
	UAbilitySystemComponent* OwnerASC
	= GetAbilitySystemComponentFromActorInfo();
	
	if (!TargetASC || !OwnerASC) return;
	/*Must have Authority*/
	if (!OwnerASC->GetOwner()->HasAuthority()) return;

	UE_LOG(LogTemp, Warning, TEXT("Applying effects to target: %s"), *Target->GetName());
	
	for (auto& SpecHandle : MakeEffectSpecsHandles())
	{
		
		TargetASC->ApplyGameplayEffectSpecToSelf(*SpecHandle.Data.Get());
	}
}

void UHeroicSlam::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
                             const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicatedEndAbility, bool bWasCancelled)
{
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicatedEndAbility, bWasCancelled);
}



