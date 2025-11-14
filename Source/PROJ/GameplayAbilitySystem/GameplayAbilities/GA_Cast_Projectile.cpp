// Fill out your copyright notice in the Description page of Project Settings.


#include "GA_Cast_Projectile.h"
#include "AbilitySystemComponent.h"
#include "PROJ/Projectiles/Projectile.h"

UGA_Cast_Projectile::UGA_Cast_Projectile()
{
	ProjectileActor = nullptr;
}

void UGA_Cast_Projectile::ActivateAbility(const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
	const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);
	if (!CommitAbility(Handle, ActorInfo, ActivationInfo))
	{
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		return;
	}
	
	Cast();
	//EndAbility(Handle, ActorInfo, ActivationInfo, true, false);
}

void UGA_Cast_Projectile::OnProjectileHit(const FHitResult& Hit)
{
	NewTargetData = new FGameplayAbilityTargetData_SingleTargetHit(Hit);
	//UE_LOG(LogTemp, Warning, TEXT("Spawned Hit: %s"), *Hit.GetActor()->GetName());
	CurrentTargetData.Add(NewTargetData);
}

void UGA_Cast_Projectile::Cast()
{
	SpawnProjectile();
	
}

void UGA_Cast_Projectile::SpawnProjectile()
{
	AActor* Avatar = GetAvatarActorFromActorInfo();
	if (!Avatar) return;

	UWorld* World = Avatar->GetWorld();
	if (!World) return;

	// can have more precise position if needed
	SpawnLocation = Avatar->GetActorLocation() + Avatar->GetActorForwardVector() * 100.f;

	// can change to camera rotation
	SpawnRotation = Avatar->GetActorRotation();

	if (GetOwningActorFromActorInfo()->HasAuthority())  // only server
	{
		ProjectileActor = World->SpawnActor<AProjectile>(ProjectileActorClass, SpawnLocation, SpawnRotation);
		if (ProjectileActor)
		{
			ProjectileActor->SetReplicates(true);
			ProjectileActor->SetReplicateMovement(true);
			ProjectileActor->OnProjectileHitDelegate.AddDynamic(this, &UGA_Cast_Projectile::OnProjectileHit);
			ProjectileActor->Caster = Avatar;
			ProjectileActor->CastedAbility = this;
			ProjectileActor->CasterASC = GetAbilitySystemComponentFromActorInfo();
			ProjectileActor->Effects = DefaultEffects;
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("Failed to spawn projectile"));
		
		}
	}
	

	
		
	
}
