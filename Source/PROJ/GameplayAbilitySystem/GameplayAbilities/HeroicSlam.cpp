// Fill out your copyright notice in the Description page of Project Settings.


#include "HeroicSlam.h"

#include "Abilities/Tasks/AbilityTask_WaitTargetData.h"
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
	if (IndicatorClass)
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
void UHeroicSlam::LaunchToTarget()
{
	ABaseCharacter* Player = Cast<ABaseCharacter>(GetAvatarActorFromActorInfo());
	if (!Player) return;

	CachedPlayer = Player;
	ESuggestProjVelocityTraceOption::Type TraceOption = ESuggestProjVelocityTraceOption::DoNotTrace;
	FCollisionResponseParams& ResponseParam = FCollisionResponseParams::DefaultResponseParam;
	TArray<AActor*> ActorsToIgnore;
	ActorsToIgnore.Add(Player);
	
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
		float OriginalMaxSpeed = Player->GetCharacterMovement()->MaxWalkSpeed;
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
	if (CachedPlayer && CachedPlayer->GetCharacterMovement()->IsMovingOnGround())
	{
		CachedPlayer->GetCharacterMovement()->MaxWalkSpeed = CachedOriginalMaxSpeed;
		GetWorld()->GetTimerManager().ClearTimer(LandingCheckTimer);
		EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
	}
}
void UHeroicSlam::OnCancel(const FGameplayAbilityTargetDataHandle& Data)
{
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
}

void UHeroicSlam::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
                             const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicatedEndAbility, bool bWasCancelled)
{
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicatedEndAbility, bWasCancelled);
}



