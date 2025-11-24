// Fill out your copyright notice in the Description page of Project Settings.


#include "Projectile.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Components/SphereComponent.h"
#include "GameplayEffectTypes.h"

// Sets default values
AProjectile::AProjectile()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	CollisionComp = CreateDefaultSubobject<USphereComponent>(TEXT("Collision"));
	RootComponent = CollisionComp;
	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	Mesh->SetupAttachment(CollisionComp);
	ProjectileMovement = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileMovement"));
	
}

// Called when the game starts or when spawned
void AProjectile::BeginPlay()
{
	Super::BeginPlay();
	Mesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	ProjectileMovement->InitialSpeed = ProjectileSpeed;
	ProjectileMovement->MaxSpeed = ProjectileSpeed;
	ProjectileMovement->bRotationFollowsVelocity = true;
	ProjectileMovement->ProjectileGravityScale = 0.f;
	ProjectileMovement->SetIsReplicated(true);
	//SetReplicates(true);  
	//SetReplicateMovement(false);
	
	CollisionComp->SetNotifyRigidBodyCollision(true);
	ProjectileMovement->Velocity = GetActorForwardVector() * ProjectileSpeed;
	CollisionComp->SetGenerateOverlapEvents(true);
	CollisionComp->SetCollisionProfileName(TEXT("Projectile"));
	CollisionComp->OnComponentBeginOverlap.AddDynamic(this, &AProjectile::OnBeginOverlap);
	CollisionComp->OnComponentHit.AddDynamic(this, &AProjectile::OnProjectileHit);
	
	GetWorldTimerManager().SetTimer(DestroyTimerHandle, this, &AProjectile::DestroySelf, ProjectileLifeTime, false);
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


void AProjectile::OnProjectileHit_Implementation(UPrimitiveComponent* HitComp, AActor* OtherActor,
                                  UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	if (ShouldSkipHit(OtherActor))
	{
		return;
	}
	
	OnProjectileHitDelegate.Broadcast(Hit);
	
	
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
	
	
		//ApplyEffectToTarget(OtherActor);
	
	
	//UE_LOG(LogTemp, Warning, TEXT("Hit %s via OnBeginOverlap"), *OtherActor->GetActorNameOrLabel());
	
	//Destroy();
	
}

