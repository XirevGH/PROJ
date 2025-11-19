// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "PROJ/AbilityActors/AbilityActor.h"
#include "Projectile.generated.h"


DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnProjectileHit, const FHitResult&, Hit);

UCLASS()
class PROJ_API AProjectile : public AAbilityActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	AProjectile();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Movement")
	class UProjectileMovementComponent* ProjectileMovement;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Components")
	UStaticMeshComponent* Mesh;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Components")
	class USphereComponent* CollisionComp;
	

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Projectile Stats")
	float ProjectileSpeed;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Projectile Stats")
	float ProjectileLifeTime;
	
	FTimerHandle DestroyTimerHandle;
	
	UPROPERTY(BlueprintAssignable)
	FOnProjectileHit OnProjectileHitDelegate;
	
	TArray<ACharacter*> OverlapCharacters;


	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UFUNCTION()
	void DestroySelf();
	
	
	
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	void OnProjectileHit(
		UPrimitiveComponent* HitComp,
		AActor* OtherActor,
		UPrimitiveComponent* OtherComp,
		FVector NormalImpulse,
		const FHitResult& Hit);
	virtual void OnProjectileHit_Implementation(
		UPrimitiveComponent* HitComp,
		AActor* OtherActor,
		UPrimitiveComponent* OtherComp,
		FVector NormalImpulse,
		const FHitResult& Hit);
	
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	void OnBeginOverlap(
	   UPrimitiveComponent* OverlappedComponent,
	   AActor* OtherActor,
	   UPrimitiveComponent* OtherComp,
	   int32 OtherBodyIndex,
	   bool bFromSweep,
	   const FHitResult& SweepResult);
	virtual void OnBeginOverlap_Implementation(
	UPrimitiveComponent* OverlappedComponent,
	AActor* OtherActor,
	UPrimitiveComponent* OtherComp,
	int32 OtherBodyIndex,
	bool bFromSweep,
	const FHitResult& SweepResult);
};

