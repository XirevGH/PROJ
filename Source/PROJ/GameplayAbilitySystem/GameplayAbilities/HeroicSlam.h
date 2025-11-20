// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BaseGameplayAbility.h"
#include "HeroicSlam.generated.h"

/**
 * 
 */
UCLASS()
class PROJ_API UHeroicSlam : public UBaseGameplayAbility
{
	GENERATED_BODY()

public:

	UHeroicSlam();

	virtual void ActivateAbility(
		const FGameplayAbilitySpecHandle Handle,
		const FGameplayAbilityActorInfo* ActorInfo,
		const FGameplayAbilityActivationInfo ActivationInfo,
		const FGameplayEventData* TriggerEventData) override;

	virtual void EndAbility(
		const FGameplayAbilitySpecHandle Handle,
		const FGameplayAbilityActorInfo* ActorInfo,
	    const FGameplayAbilityActivationInfo ActivationInfo,
	    bool bReplicatedEndAbility, bool bWasCancelled) override;
};
