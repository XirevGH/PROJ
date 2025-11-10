// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BaseGameplayAbility.h"
#include "GA_CastProjectile.generated.h"

/**
 * 
 */
UCLASS()
class PROJ_API UGA_CastProjectile : public UBaseGameplayAbility
{
	GENERATED_BODY()
	public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	AActor* ProjectileActor;

	UPROPERTY(EditAnywhere,BlueprintReadWrite)
	TSubclassOf<AActor> ProjectileActorClass;
	
	UGA_CastProjectile();

	protected:
	
	virtual  void ActivateAbility(const FGameplayAbilitySpecHandle Handle,
		const FGameplayAbilityActorInfo* ActorInfo,
		const FGameplayAbilityActivationInfo ActivationInfo,
		const FGameplayEventData* TriggerEventData) override;
	

	UFUNCTION(BlueprintImplementableEvent)
	void OntargetReady(const FGameplayAbilityTargetDataHandle& TargetData);

	UFUNCTION(BlueprintCallable)
	void Cast();

};
