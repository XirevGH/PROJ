// Fill out your copyright notice in the Description page of Project Settings.


#include "Projectile_SpacetimeAnomaly.h"
#include "GameFramework/Character.h"

// Sets default values
AProjectile_SpacetimeAnomaly::AProjectile_SpacetimeAnomaly()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
}

// Called when the game starts or when spawned
void AProjectile_SpacetimeAnomaly::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AProjectile_SpacetimeAnomaly::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	//for (ACharacter* Actor : OverlapCharacters)
	//{
	//	if (!Actor) continue;

	//	UPrimitiveComponent* Prim = Cast<UPrimitiveComponent>(Actor->GetRootComponent());
		//if (!Prim || !Prim->IsSimulatingPhysics()) continue;

	//	FVector Dir = GetActorLocation() - Actor->GetActorLocation(); // direction to black hole
	//	float Distance = Dir.Size();
	//	Dir.Normalize();
	//	UE_LOG(LogTemp, Warning, TEXT("Prim->AddForce to %s"),*Actor->GetName() );
	//	float Strength = FMath::Clamp(1000000.f / (Distance * Distance), 0.f, 500000.f); // inverse square
	//	Prim->AddForce(Dir * Strength);
	//}
	
}

void AProjectile_SpacetimeAnomaly::OnProjectileHit_Implementation(UPrimitiveComponent* HitComp, AActor* OtherActor,
								  UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	Super::OnProjectileHit_Implementation( HitComp,  OtherActor,
								  OtherComp,  NormalImpulse, Hit);
	
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
	
	//Destroy();
}


void AProjectile_SpacetimeAnomaly::OnBeginOverlap_Implementation(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor == Caster)
	{
		UE_LOG(LogTemp, Warning, TEXT("Do nothing because hit Caster"));
		return;
	}

	if (OtherActor->IsA(StaticClass()))
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
		ApplyEffectToTarget(OtherActor);
	}
	
	//UE_LOG(LogTemp, Warning, TEXT("Hit %s via OnBeginOverlap"), *OtherActor->GetActorNameOrLabel());
	
	//Destroy();
	
}


