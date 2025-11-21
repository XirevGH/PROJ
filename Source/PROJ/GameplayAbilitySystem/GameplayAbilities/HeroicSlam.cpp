// Fill out your copyright notice in the Description page of Project Settings.


#include "HeroicSlam.h"

#include "AbilitySystemGlobals.h"
#include "Abilities/Tasks/AbilityTask_WaitTargetData.h"
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
	
	ABaseCharacter* Player = Cast<ABaseCharacter>(ActorInfo->AvatarActor.Get());
	if (!Player)
	{
		UE_LOG(LogTemp, Warning, TEXT("Player is null"));
		return;
	}
	CachedPlayer = Player;
	/*Shoot up*/
	FVector Direction = Player->GetActorUpVector();
	Player->LaunchCharacter(Direction * 1000.f, true,true);

	/*Get & Check player controller*/
	APlayerController* PC = Cast<APlayerController>(Player->GetController());
	if (!PC)
	{
		UE_LOG(LogTemp, Warning, TEXT("Player controller is null"));
		return;
	}
	if (!IndicatorClass)
	{
		UE_LOG(LogTemp, Warning, TEXT("IndicatorClass is null"));
		return;
	}
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
		const FGameplayAbilityTargetData* TargetData = Data.Get(0);
		if (TargetData)
		{
			if (const FGameplayAbilityTargetData_SingleTargetHit* HitData =
				static_cast<const FGameplayAbilityTargetData_SingleTargetHit*>(TargetData))
			{
				TargetLocation = HitData->HitResult.ImpactPoint;
				UE_LOG(LogTemp, Warning, TEXT("TargetLocation from Hit Result: %f,%f,%f"), TargetLocation.X,TargetLocation.Y,TargetLocation.Z);
			}
			else if (const FGameplayAbilityTargetData_LocationInfo* LocationData =
				static_cast<const FGameplayAbilityTargetData_LocationInfo*>(TargetData))
			{
				TargetLocation = LocationData->GetEndPoint();
				UE_LOG(LogTemp, Warning, TEXT("TargetLocation from GetEndPoint: %f,%f,%f"), TargetLocation.X,TargetLocation.Y,TargetLocation.Z);
			}
			/*Call Launch Logic*/
			LaunchToTarget();
		}
	}
}

void UHeroicSlam::Multicast_PlaySlamEffects_Implementation(FVector Location)
{
	if (SlamVFX)
		UNiagaraFunctionLibrary::SpawnSystemAtLocation(
			GetWorld(),
			SlamVFX,
			Location
		);
}

void UHeroicSlam::LaunchToTarget()
{
	ESuggestProjVelocityTraceOption::Type TraceOption = ESuggestProjVelocityTraceOption::DoNotTrace;
	FCollisionResponseParams& ResponseParam = FCollisionResponseParams::DefaultResponseParam;
	TArray<AActor*> ActorsToIgnore;
	ActorsToIgnore.Add(CachedPlayer);
	
	FVector Start = CachedPlayer->GetActorLocation();
	FVector End = TargetLocation;
	
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
	
	/*Runs if SuggestProjVel is true*/
	if (bHasSolution)
	{
		/*Get players current MaxWalkSpeed*/
		float OriginalMaxSpeed = CachedPlayer->GetCharacterMovement()->MaxWalkSpeed;
		/*Raises player MaxWalkSpeed*/
		CachedPlayer->GetCharacterMovement()->MaxWalkSpeed = FMath::Max(LaunchVelocity.Length() + 100.f, OriginalMaxSpeed);
		CachedPlayer->GetCharacterMovement()->StopMovementImmediately();
		/*Launch player in an arc*/
		CachedPlayer->LaunchCharacter(LaunchVelocity, true, true);
		/*Save the cached MaxWalkSpeed*/
		CachedOriginalMaxSpeed = OriginalMaxSpeed;

		/*Set a timer to check every 0.1 sec to see if the player landed to restore MS*/
		GetWorld()->GetTimerManager().SetTimer(
		LandingCheckTimer,
		this,
		&UHeroicSlam::LandingCheck,
		0.1f,
		true);
	}
}
void UHeroicSlam::LandingCheck()
{
	if (!CachedPlayer->HasAuthority()) return;
	if (!CachedPlayer) return;
	if (!CachedPlayer->GetCharacterMovement()->IsMovingOnGround()) return;

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
	
	CachedPlayer->GetCharacterMovement()->MaxWalkSpeed = CachedOriginalMaxSpeed;
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



