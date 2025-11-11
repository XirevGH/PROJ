// Fill out your copyright notice in the Description page of Project Settings.


#include "Projectile.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Components/SphereComponent.h"

// Sets default values
AProjectile::AProjectile()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	RootComponent = Mesh;
	ProjectileMovement = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileMovement"));
	CollisionComp = CreateDefaultSubobject<USphereComponent>(TEXT("Collision"));
}

// Called when the game starts or when spawned
void AProjectile::BeginPlay()
{
	Super::BeginPlay();
	 // 0 = no gravity
	Mesh->SetNotifyRigidBodyCollision(true);
	Mesh->SetSimulatePhysics(false);
	ProjectileMovement->InitialSpeed = ProjectileSpeed;
	ProjectileMovement->MaxSpeed = ProjectileSpeed;
	ProjectileMovement->bRotationFollowsVelocity = true;
	ProjectileMovement->ProjectileGravityScale = 0.f;
	ProjectileMovement->SetIsReplicated(true);
	SetReplicates(true);  
	SetReplicateMovement(false);
	
	ProjectileMovement->Velocity = GetActorForwardVector() * ProjectileSpeed;
	Mesh->OnComponentHit.AddDynamic(this, &AProjectile::OnProjectileHit);
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

void AProjectile::IgnoreCaster(AActor* Caster) const
{
	Mesh->IgnoreActorWhenMoving(Caster, true);
}



void AProjectile::OnProjectileHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp,
                                 FVector NormalImpulse, const FHitResult& Hit)
{
	if(Cast<AActor>(OtherActor) != nullptr )
	{
		OnProjectileHitDelegate.Broadcast(Hit);
	}
	//UE_LOG(LogTemp, Warning, TEXT("Hit"));
	Destroy();
}

