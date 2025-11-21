// Fill out your copyright notice in the Description page of Project Settings.


#include "HeroicSlam.h"

#include "Abilities/Tasks/AbilityTask_WaitConfirmCancel.h"
#include "Components/DecalComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "PROJ/BaseCharacter.h"
#include "PROJ/GameplayAbilitySystem/Indicators/Indicator.h"

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

	/*Spawn params*/
	FActorSpawnParameters SpawnParams;
	SpawnParams.Owner = Player;
	SpawnParams.Instigator = Cast<APawn>(Player);

	/*Spawn*/
	Indicator = GetWorld()->SpawnActor<AIndicator>(
		IndicatorClass,
		Player->GetActorLocation(),
		FRotator::ZeroRotator,
		SpawnParams);

	/*Check Indicator*/
	if (!Indicator)
	{
		UE_LOG(LogTemp, Warning, TEXT("Failed to spawn Indicator"));
		return;
	}

	/*Get & Check player controller*/
	APlayerController* PC = Cast<APlayerController>(Player->GetController());
	if (!PC)
	{
		UE_LOG(LogTemp, Warning, TEXT("Player controller is null"));
		return;
	}

	/*Setup Indicator*/
	Indicator->CasterController = PC;
	Indicator->Caster = Cast<APawn>(Player);
	Indicator->MaxRange = Range;
	Indicator->Decal->DecalSize = FVector(128.f,Radius,Radius);

	/*Task & delegate setup*/
	UAbilityTask_WaitConfirmCancel* Task = UAbilityTask_WaitConfirmCancel::WaitConfirmCancel(this);
	Task->OnConfirm.AddDynamic(this, &UHeroicSlam::OnConfirm);
	Task->OnConfirm.AddDynamic(this, &UHeroicSlam::OnCancel);
	Task->ReadyForActivation();

	UE_LOG(LogTemp, Warning, TEXT("Task %s"), Task->IsValidLowLevel() ? TEXT("Successfully activated") : TEXT("Failed to activate"));
}

void UHeroicSlam::OnConfirm()
{
	if (!Indicator) return;

	/*Get Location Marked With Indicator*/
	TargetLocation = Indicator->GetActorLocation();

	Indicator->Destroy();
	Indicator = nullptr;
	
	/*Call Launch Logic*/
	LaunchToTarget();
}
void UHeroicSlam::LaunchToTarget()
{
	ABaseCharacter* Player = Cast<ABaseCharacter>(GetAvatarActorFromActorInfo());
	if (!Player) return;

	FVector Start = Player->GetActorLocation();
	FVector End = TargetLocation;

	float DesiredArcHeight = 400.f;
	FVector FlatEnd = End;
	FlatEnd.Z = Start.Z;

	FVector Direction = (FlatEnd - Start);
	float DistanceXY = Direction.Size2D();
	Direction.Normalize();

	float JumpTime = 0.5f;

	FVector Velocity;
	Velocity.X = Direction.X * DistanceXY/JumpTime;
	Velocity.Y = Direction.Y * DistanceXY/JumpTime;

	Velocity.Z = (2.f * DesiredArcHeight) / JumpTime;
	
	/*FVector LaunchVelocity;
	bool bHasSolution = UGameplayStatics::SuggestProjectileVelocity_CustomArc(
		this,
		LaunchVelocity,
		Start,
		End,
		0.5f);*/
	
	
		Player->GetCharacterMovement()->StopMovementImmediately();

		Player->LaunchCharacter(Velocity, true, true);
}
void UHeroicSlam::OnCancel()
{
	
}
void UHeroicSlam::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicatedEndAbility, bool bWasCancelled)
{
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicatedEndAbility, bWasCancelled);
}
