// Fill out your copyright notice in the Description page of Project Settings.


#include "GA_Cast_Projectile.h"
#include "AbilitySystemComponent.h"
#include "PROJ/AbilityActors/Projectiles/Projectile.h"
#include "PROJ/Data/ProjectileDataAsset.h"
#include "GameFramework/PlayerController.h"

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
		
		SpawnLocation = SpawnTransform.GetLocation();
		SpawnRotation = GetProjectileSpawnRotation(SpawnLocation);
		//could set rotation to socket rotation, but it won't always be Avatar->GetActorRotation()
		//SpawnRotation = SpawnTransform.GetRotation().Rotator();
		
		
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
		SpawnRotation = GetProjectileSpawnRotation(SpawnLocation);
		
		// can change to camera rotation
		UE_LOG(LogTemp, Warning, TEXT("Socket %s does not exist!"), *SpawnSocketName.ToString())
	}

	if (!ProjectileData)
	{
		UE_LOG(LogTemp, Warning, TEXT("projectile data does not exist!"))
		return;
	}
		
		
	ProjectileActor = World->SpawnActor<AProjectile>(ProjectileData->ProjectileActorClass, SpawnLocation, SpawnRotation);
	if (ProjectileActor)
	{
		InitializeAbilityActor(ProjectileActor);
		
		//UE_LOG(LogTemp, Warning, TEXT("Data asset in ability %s"), ProjectileData ? TEXT("is valid now"): TEXT("Not valid now"));
		ProjectileActor->InitializeProjectile(ProjectileData);
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Failed to spawn projectile"));
	}
	
}

FRotator UGA_Cast_Projectile::GetProjectileSpawnRotation(FVector StartLocation)
{
	AActor* Avatar = GetAvatarActorFromActorInfo();
	
	FVector Start = Avatar->GetActorLocation();
	
	float Distance = 100000.f;

	FVector Forward = Avatar->GetActorForwardVector();
	FVector End = Start + (Forward * Distance);
	
	FHitResult Hit;
	FCollisionQueryParams Params;
	Params.AddIgnoredActor(Avatar);

	bool bHit = GetWorld()->LineTraceSingleByChannel(
		Hit,
		Start,
		End,
		ECC_Visibility,
		Params
	);

	if (bHit)
	{
		End = Hit.ImpactPoint;
	}
	DrawDebugLine(GetWorld(), StartLocation, End, FColor::Red, false, 2.f, 0, 2.f);
	return (End - StartLocation).Rotation();
}
