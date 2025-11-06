// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbility.h"
#include "GameplayTagContainer.h"
#include "GP_Dash.generated.h"

/**
 * 
 */
UCLASS()
class PROJ_API UGP_Dash : public UGameplayAbility
{
	GENERATED_BODY()
	
public:
	UGP_Dash();

	FGameplayTag DashTag;
	FGameplayTag StunnedTag; 
	FGameplayTag CooldownTag; 
	
	virtual void ActivateAbility(
		const FGameplayAbilitySpecHandle Handle,
		const FGameplayAbilityActorInfo* ActorInfo,
		const FGameplayAbilityActivationInfo ActivationInfo,
		const FGameplayEventData* TriggerEventData) override;
};

