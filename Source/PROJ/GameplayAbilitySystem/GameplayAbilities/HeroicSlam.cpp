// Fill out your copyright notice in the Description page of Project Settings.


#include "HeroicSlam.h"

#include "Abilities/Tasks/AbilityTask_WaitTargetData.h"
#include "Engine/OverlapResult.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "PROJ/Characters/BaseCharacter.h"



UHeroicSlam::UHeroicSlam()
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
}

void UHeroicSlam::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
                                  const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);
	
	/*Get and Check CachedPlayer*/
	CachedPlayer = Cast<ABaseCharacter>(ActorInfo->AvatarActor.Get());
	if (!CachedPlayer) return;
	
	if (!CommitAbility(Handle, ActorInfo, ActivationInfo))
	{
		EndAbility(Handle, ActorInfo, ActivationInfo, true, false);
		return;
	}
	LaunchToTarget();
}

void UHeroicSlam::RestoreAirFriction()
{
	/*Play VFX*/
	if (SlamVfx && CachedPlayer)
	{
		UNiagaraFunctionLibrary::SpawnSystemAtLocation(
				GetWorld(),
				SlamVfx,
				CachedPlayer->GetActorLocation(),
				FRotator::ZeroRotator,
				FVector(10.f));
	}
	auto* Move = CachedPlayer->GetCharacterMovement();
	Move->MaxWalkSpeed = CachedOriginalMaxSpeed;
	Move->AirControl = OriginalAirControl;
	Move->BrakingDecelerationFalling = OriginalBraking;
	Move->FallingLateralFriction = OriginalFriction;
	Move->SetMovementMode(MOVE_Walking);
}

void UHeroicSlam::LaunchToTarget()
{
	FVector Forward = CachedPlayer->GetActorForwardVector();
	FVector Start = CachedPlayer->GetActorLocation();
	FVector End = Start + Forward * JumpDistance;
	/*Because Target location is on ground level and player start is above ground(Better calculation imo*TEST*)*/
	End.Z += /*CachedPlayer->GetCapsuleComponent()->GetScaledCapsuleHalfHeight() +*/ 500.f;
	

	FVector LaunchVelocity;
	bool bHasSolution = UGameplayStatics::SuggestProjectileVelocity_CustomArc(
			this,
			LaunchVelocity,
			Start,
			End,
			0.f,
			ArcParam);
	
	if (!bHasSolution)
	{
		TargetLocation = Start + (End - Start).GetSafeNormal() * 500.f;
		UE_LOG(LogTemp, Warning, TEXT("NoSolution"));
	}
	
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
		TSet<AActor*> HitActors;
		for (auto& Result : Overlaps)
		{
			AActor* HitActor = Result.GetActor();
			if (!HitActor || HitActor == CachedPlayer) continue;
			if (HitActors.Contains(HitActor)) continue;

			HitActors.Add(HitActor);
			ApplyEffectsToTarget(HitActor);
		}
	}
	/*Reset all movement attributes*/
	RestoreAirFriction();
	/*Reset timer*/
	GetWorld()->GetTimerManager().ClearTimer(LandingCheckTimer);
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
}




