// Fill out your copyright notice in the Description page of Project Settings.


#include "Projectile.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Components/SphereComponent.h"
#include "AbilitySystemComponent.h"
#include "AbilitySystemGlobals.h"
#include "./PROJ/GameplayAbilitySystem/GameplayAbilities/BaseGameplayAbility.h"

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
	SetReplicates(true);  
	SetReplicateMovement(false);
	
	CollisionComp->SetNotifyRigidBodyCollision(true);
	ProjectileMovement->Velocity = GetActorForwardVector() * ProjectileSpeed;
	CollisionComp->SetGenerateOverlapEvents(true);
	
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

void AProjectile::IgnoreCaster() const
{
	Mesh->IgnoreActorWhenMoving(Caster, true);
	CollisionComp->IgnoreActorWhenMoving(Caster, true);
}

void AProjectile::OnProjectileHit(UPrimitiveComponent* HitComp, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	if (OtherActor == Caster)
	{
		UE_LOG(LogTemp, Warning, TEXT("Do nothing because hit Caster"));
		return;
	}

	if(Cast<AActor>(OtherActor) != nullptr )
	{
		OnProjectileHitDelegate.Broadcast(Hit);
	}
	UE_LOG(LogTemp, Warning, TEXT("Hit %s via OnHit"), *OtherActor->GetActorNameOrLabel());
	
	Destroy();
}


void AProjectile::OnBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor == Caster)
	{
		UE_LOG(LogTemp, Warning, TEXT("Do nothing because hit Caster"));
		return;
	}

	if (OtherActor->IsA(this->GetClass()))
	{
		UE_LOG(LogTemp, Warning, TEXT("Do nothing because hit same type of projectile"));
		return;
	}
	
	if(Cast<AActor>(OtherActor) != nullptr )
	{
		OnProjectileHitDelegate.Broadcast(SweepResult);
	}
	if (CasterASC && CastedAbility)
	{
		UAbilitySystemComponent* TargetASC = UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(OtherActor);
		if (HasAuthority() && TargetASC)
		{
			UE_LOG(LogTemp, Warning, TEXT("Target ASC is from: %s"), *TargetASC->GetAvatarActor()->GetName());
			for (auto& EffectClass : Effects)
			{
				FGameplayEffectSpecHandle SpecHandle = CasterASC->MakeOutgoingSpec(EffectClass, CastedAbility->GetAbilityLevel(), CasterASC->MakeEffectContext());
				SpecHandle.Data->SetSetByCallerMagnitude(FGameplayTag::RequestGameplayTag(FName("Data.Damage")), CastedAbility->BaseDamage);
				TargetASC->ApplyGameplayEffectSpecToSelf(*SpecHandle.Data.Get());
			}
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("No target ASC"));
		}
	}
	
	//UE_LOG(LogTemp, Warning, TEXT("Hit %s via OnBeginOverlap"), *OtherActor->GetActorNameOrLabel());
	
	Destroy();
	
}

