// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "ProjectileDataAsset.generated.h"

class AProjectile;
/**
 * 
 */
UCLASS()
class PROJ_API UProjectileDataAsset : public UPrimaryDataAsset
{
	GENERATED_BODY()
	
public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Particles")
	UParticleSystem* ProjectileParticle;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Particles")
	UParticleSystem* WorldHitParticle;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Particles")
	UParticleSystem* CharacterHitParticle;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Particles")
	UParticleSystem* NoHitParticle;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Particles / Ability")
	UParticleSystem* MuzzleFlashParticle;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Particles / Ability")
	UParticleSystem* CastParticle;
	
	
	UPROPERTY(EditDefaultsOnly,BlueprintReadWrite)
	FName SpawnSocketName;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Class")
	TSubclassOf<AProjectile> ProjectileActorClass;
	
	

	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Projectile Stats")
	float ProjectileSpeed;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Projectile Stats")
	float ProjectileLifeTime;
};
