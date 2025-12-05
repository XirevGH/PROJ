// Fill out your copyright notice in the Description page of Project Settings.


#include "GA_Cast_Projectile.h"
#include "AbilitySystemComponent.h"
#include "PROJ/AbilityActors/Projectiles/Projectile.h"

UGA_Cast_Projectile::UGA_Cast_Projectile()
{
	ProjectileActor = nullptr;
}

void UGA_Cast_Projectile::ActivateAbility(const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
	const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);
}


void UGA_Cast_Projectile::Cast()
{
	SpawnProjectile();
	
}

void UGA_Cast_Projectile::SpawnProjectile()
{

	if (!GetOwningActorFromActorInfo()->HasAuthority())  // only server
	{
		return;
	}
	
	AActor* Avatar = GetAvatarActorFromActorInfo();
	if (!Avatar) return;

	UWorld* World = Avatar->GetWorld();
	if (!World) return;
	
	
	if (GetActorInfo().SkeletalMeshComponent->DoesSocketExist(SpawnSocketName))
	{
		// Get the transform of the socket
		FTransform SpawnTransform = GetActorInfo().SkeletalMeshComponent->GetSocketTransform(SpawnSocketName);

		// Optional: get location
		SpawnLocation = SpawnTransform.GetLocation();
		//could set rotation to socket rotation, but it won't always be Avatar->GetActorRotation()
		SpawnRotation = Avatar->GetActorRotation();

		// Debug: Draw a sphere at the spawn location
		//DrawDebugSphere(GetWorld(), SpawnLocation,10.f,12, FColor::Red,true);
	}
	else
	{ 
		// if no socket spawn here
		float HeightOffset = 30.f; // whatever you want
		SpawnLocation = Avatar->GetActorLocation()
			+ Avatar->GetActorForwardVector()
			+ FVector(0.f, 0.f, HeightOffset);

		// can change to camera rotation
		SpawnRotation = Avatar->GetActorRotation();
		UE_LOG(LogTemp, Warning, TEXT("Socket %s does not exist!"), *SpawnSocketName.ToString())
	}


		
	ProjectileActor = World->SpawnActor<AProjectile>(ProjectileActorClass, SpawnLocation, SpawnRotation);
	if (ProjectileActor)
	{
		InitializeAbilityActor(ProjectileActor);
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Failed to spawn projectile"));
	}
}
