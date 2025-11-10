// Fill out your copyright notice in the Description page of Project Settings.


#include "GA_CastProjectile.h"

UGA_CastProjectile::UGA_CastProjectile()
{
	ProjectileActor = CreateDefaultSubobject<AActor>(TEXT("Projectile"));
}

void UGA_CastProjectile::ActivateAbility(const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
	const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);
	Cast();
	EndAbility(Handle, ActorInfo, ActivationInfo, true, false);
}

void UGA_CastProjectile::Cast()
{
	AActor* Avatar = GetAvatarActorFromActorInfo();
	if (!Avatar || !ProjectileActor) return;

	UWorld* World = Avatar->GetWorld();
	if (!World) return;

	FVector SpawnLocation = Avatar->GetActorLocation() + Avatar->GetActorForwardVector() * 100.f;
	FRotator SpawnRotation = Avatar->GetActorRotation();

	 ProjectileActor = World->SpawnActor<AActor>(ProjectileActorClass, SpawnLocation, SpawnRotation);

	if (ProjectileActor)
	{
		UE_LOG(LogTemp, Warning, TEXT("Spawned projectile: %s"), *ProjectileActor->GetName());
	}
}