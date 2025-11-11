// Fill out your copyright notice in the Description page of Project Settings.


#include "GA_CastProjectile.h"
#include "AbilitySystemComponent.h"
#include "PROJ/Projectiles/Projectile.h"

UGA_CastProjectile::UGA_CastProjectile()
{
	ProjectileActor = nullptr;
}

void UGA_CastProjectile::ActivateAbility(const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
	const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);
	Cast();
	EndAbility(Handle, ActorInfo, ActivationInfo, true, false);
}

void UGA_CastProjectile::OnProjectileHit(const FHitResult& Hit)
{
	NewTargetData = new FGameplayAbilityTargetData_SingleTargetHit(Hit);
	UE_LOG(LogTemp, Warning, TEXT("Spawned projectile: %s"), *Hit.GetActor()->GetName());
	CurrentTargetData.Add(NewTargetData);
}

void UGA_CastProjectile::Cast()
{
	AActor* Avatar = GetAvatarActorFromActorInfo();
	if (!Avatar) return;

	UWorld* World = Avatar->GetWorld();
	if (!World) return;

	FVector SpawnLocation = Avatar->GetActorLocation() + Avatar->GetActorForwardVector() * 100.f;
	FRotator SpawnRotation = Avatar->GetActorRotation();

	 ProjectileActor = World->SpawnActor<AProjectile>(ProjectileActorClass, SpawnLocation, SpawnRotation);

	if (ProjectileActor)
	{
		//ProjectileActor->IgnoreCaster(Avatar);
		UE_LOG(LogTemp, Warning, TEXT("Spawned projectile: %s"), *ProjectileActor->GetName());
		ProjectileActor->OnProjectileHitDelegate.AddDynamic(this, &UGA_CastProjectile::OnProjectileHit);
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Failed to spawn projectile"));
	}
}
