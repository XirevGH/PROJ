// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BaseGameplayAbility.h"
#include "GA_CastProjectile.generated.h"

/**
 * 
 */
class AProjectile;
UCLASS()
class PROJ_API UGA_CastProjectile : public UBaseGameplayAbility
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

	UPROPERTY(EditAnywhere,BlueprintReadWrite)
	TSubclassOf<AProjectile> ProjectileActorClass;
	
	UGA_CastProjectile();

	protected:
	
	virtual  void ActivateAbility(const FGameplayAbilitySpecHandle Handle,
		const FGameplayAbilityActorInfo* ActorInfo,
		const FGameplayAbilityActivationInfo ActivationInfo,
		const FGameplayEventData* TriggerEventData) override;
	

	UFUNCTION(BlueprintImplementableEvent)
	void OntargetReady(const FGameplayAbilityTargetDataHandle& TargetData);

	UFUNCTION()
	void OnProjectileHit(const FHitResult& Hit);

	UFUNCTION(BlueprintCallable)
	void Cast();

	UFUNCTION(BlueprintCallable)
	void SpawnProjectile();

	
	
	private:
	UPROPERTY()
	FGameplayAbilityTargetDataHandle CurrentTargetData;

	FGameplayAbilityTargetData_SingleTargetHit* NewTargetData;

};
