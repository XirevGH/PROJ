// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbility.h"
#include "GA_Basic_Attack.generated.h"

/**
 * 
 */
UCLASS()
class PROJ_API UGA_Basic_Attack : public UGameplayAbility
{
	GENERATED_BODY()

	public:
	UGA_Basic_Attack();

protected:
	virtual  void ActivateAbility(const FGameplayAbilitySpecHandle Handle,
		const FGameplayAbilityActorInfo* ActorInfo,
		const FGameplayAbilityActivationInfo ActivationInfo,
		const FGameplayEventData* TriggerEventData) override;
	

	UFUNCTION(BlueprintImplementableEvent)
	void OntargetReady(const FGameplayAbilityTargetDataHandle& TargetData);

	UFUNCTION(BlueprintCallable)
	void Attack();

};
