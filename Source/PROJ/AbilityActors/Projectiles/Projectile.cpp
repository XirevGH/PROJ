// Fill out your copyright notice in the Description page of Project Settings.


#include "Projectile.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Components/SphereComponent.h"
#include "GameplayEffectTypes.h"
#include "Kismet/GameplayStatics.h"
#include "Net/UnrealNetwork.h"
#include "Particles/ParticleSystemComponent.h"
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
	InitializeProjectile(ProjectileData);
}

void AProjectile::GetLifetimeReplicatedProps(TArray< FLifetimeProperty >& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(AProjectile, ProjectileData);
}

void AProjectile::InitializeProjectile(UProjectileDataAsset* InData)
{
	ProjectileData = InData;
	ProjectileParticle->SetTemplate(InData->ProjectileParticle);
	ProjectileMovement->InitialSpeed = InData->ProjectileSpeed;
	ProjectileMovement->MaxSpeed = InData->ProjectileSpeed;
	ProjectileMovement->bRotationFollowsVelocity = true;
	ProjectileMovement->ProjectileGravityScale = 0.f;
	

	
	CollisionComp->SetNotifyRigidBodyCollision(true);
	ProjectileMovement->Velocity = GetActorForwardVector() * InData->ProjectileSpeed;
	CollisionComp->SetGenerateOverlapEvents(true);
	CollisionComp->SetCollisionProfileName(TEXT("Projectile"));
	CollisionComp->OnComponentBeginOverlap.AddDynamic(this, &AProjectile::OnBeginOverlap);
	CollisionComp->OnComponentHit.AddDynamic(this, &AProjectile::OnProjectileHit);
	GetWorldTimerManager().SetTimer(DestroyTimerHandle, this, &AProjectile::DestroySelf, InData->ProjectileLifeTime, false);
}

void AProjectile::OnProjectileHit_Implementation(UPrimitiveComponent* HitComp, AActor* OtherActor,
                                                 UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	if (ShouldSkipHit(OtherActor))
	{
		return;
	}
	
	OnProjectileHitDelegate.Broadcast(Hit);

	UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), ProjectileData->WorldHitParticle,  Hit.ImpactPoint,
	Hit.ImpactNormal.Rotation());
	
	UE_LOG(LogTemp, Warning, TEXT("Hit %s via OnHit"), *OtherActor->GetActorNameOrLabel());
	
	//Destroy();
}


void AProjectile::OnBeginOverlap_Implementation(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (ShouldSkipHit(OtherActor))
	{
		return;
	}
	
	OnProjectileHitDelegate.Broadcast(SweepResult);
	
	UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), ProjectileData->CharacterHitParticle, GetActorLocation());
		//ApplyEffectToTarget(OtherActor);
	
	
	//UE_LOG(LogTemp, Warning, TEXT("Hit %s via OnBeginOverlap"), *OtherActor->GetActorNameOrLabel());
	
	//Destroy();
	
}

