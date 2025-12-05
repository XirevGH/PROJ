// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BaseGameplayAbility.h"

#include "GA_Cast_Projectile.generated.h"

class UProjectileDataAsset;
/**
 * 
 */
class AProjectile;
UCLASS()
class PROJ_API UGA_Cast_Projectile : public UBaseGameplayAbility
{
	GENERATED_BODY()
	public:
	UPROPERTY()
	AProjectile* ProjectileActor;

	//Can change to socket later if needed
	UPROPERTY()
	FVector SpawnLocation;
	UPROPERTY()
	FRotator SpawnRotation;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="ProjectileData")
	UProjectileDataAsset* ProjectileData;
	
	UPROPERTY(EditAnywhere,BlueprintReadWrite)
	FName SpawnSocketName;
	
	UGA_Cast_Projectile();

	protected:
	
	virtual  void ActivateAbility(const FGameplayAbilitySpecHandle Handle,
		const FGameplayAbilityActorInfo* ActorInfo,
		const FGameplayAbilityActivationInfo ActivationInfo,
		const FGameplayEventData* TriggerEventData) override;
	
	UFUNCTION(BlueprintCallable)
	void Cast();

	UFUNCTION(BlueprintCallable)
	void SpawnProjectile();

	private:
	UPROPERTY()
	FGameplayAbilityTargetDataHandle CurrentTargetData;

	FGameplayAbilityTargetData_SingleTargetHit* NewTargetData;

};
