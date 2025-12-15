// Fill out your copyright notice in the Description page of Project Settings.


#include "Projectile.h"

#include "AbilitySystemComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Components/SphereComponent.h"
#include "GameplayEffectTypes.h"
#include "Kismet/GameplayStatics.h"
#include "Net/UnrealNetwork.h"
#include "Particles/ParticleSystemComponent.h"
#include "ProfilingDebugging/CookStats.h"
#include "PROJ/Data/ProjectileDataAsset.h"

// Sets default values
AProjectile::AProjectile()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	CollisionComp = CreateDefaultSubobject<USphereComponent>(TEXT("Collision"));
	RootComponent = CollisionComp;
	ProjectileParticle = CreateDefaultSubobject<UParticleSystemComponent>(TEXT("Projectile"));
	ProjectileParticle->SetupAttachment(CollisionComp);
	ProjectileMovement = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileMovement"));
}

// Called when the game starts or when spawned
void AProjectile::BeginPlay()
{
	Super::BeginPlay();
	PrimaryActorTick.bCanEverTick = true;
	SetReplicateMovement(true);
	ProjectileMovement->SetIsReplicated(true);


}


// Called every frame
void AProjectile::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AProjectile::DestroySelf()
{
	Destroy();
}
void AProjectile::OnRep_ProjectileData()
{
	ProjectileParticle->SetTemplate(ProjectileData->ProjectileParticle);
	ProjectileMovement->InitialSpeed = ProjectileData->ProjectileSpeed;
	ProjectileMovement->MaxSpeed = ProjectileData->ProjectileSpeed;
	ProjectileMovement->bRotationFollowsVelocity = true;
	ProjectileMovement->ProjectileGravityScale = 0.f;
	

	
	CollisionComp->SetNotifyRigidBodyCollision(true);
	ProjectileMovement->Velocity = GetActorForwardVector() * ProjectileData->ProjectileSpeed;
	CollisionComp->SetGenerateOverlapEvents(true);
	CollisionComp->SetCollisionProfileName(TEXT("Projectile"));
	CollisionComp->OnComponentBeginOverlap.AddDynamic(this, &AProjectile::OnBeginOverlap);
	CollisionComp->OnComponentHit.AddDynamic(this, &AProjectile::OnProjectileHit);
	SetLifeSpan(ProjectileData->ProjectileLifeTime);
	GetWorldTimerManager().SetTimer(DestroyTimerHandle, this, &AProjectile::DestroySelf, ProjectileData->ProjectileLifeTime, false);
}

void AProjectile::GetLifetimeReplicatedProps(TArray< FLifetimeProperty >& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(AProjectile, ProjectileData);
}

void AProjectile::InitializeProjectile(UProjectileDataAsset* InData)
{
	if (!HasAuthority()) return;
	ProjectileData = InData;
	OnRep_ProjectileData();
}

void AProjectile::OnProjectileHit_Implementation(UPrimitiveComponent* HitComp, AActor* OtherActor,
                                                 UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	if (!HasAuthority())
	{
		return;
	}
	
	if (!ShouldSkipHit(OtherActor))
	{
		FVector IncomingDirection = -ProjectileMovement->Velocity.GetSafeNormal();
		 MulticastSpawnImpactFX(ProjectileData->WorldHitParticle,  Hit.ImpactPoint,
		 IncomingDirection.Rotation());
	}
}


void AProjectile::OnBeginOverlap_Implementation(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{

if (!HasAuthority())
	{
		return;
	}

	if (!ShouldSkipHit_Implementation(OtherActor))
	{
		MulticastSpawnImpactFX(ProjectileData->CharacterHitParticle, GetActorLocation(), GetActorRotation());
	}
}

void AProjectile::MulticastSpawnImpactFX_Implementation(UParticleSystem* Particle, FVector Location, FRotator Rotation)
{
	 UParticleSystemComponent* HitEffect =	UGameplayStatics::SpawnEmitterAtLocation(GetWorld(),  Particle,  Location,
	Rotation);
}

