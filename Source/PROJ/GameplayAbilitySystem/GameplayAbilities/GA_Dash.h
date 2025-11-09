// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BaseGameplayAbility.h"
#include "GameplayTagContainer.h"
#include "GameplayEffect.h"
#include "GA_Dash.generated.h"

class UGE_BaseCooldown;
/**
 * 
 */
UCLASS()
class PROJ_API UGA_Dash : public UBaseGameplayAbility
{
	GENERATED_BODY()
	
public:
	UGA_Dash();

	// UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Tags")
	// FGameplayTag DashTag;
	//
	// UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Tags")
	// FGameplayTag StunnedTag;
	//
	// UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Tags")
	// FGameplayTag CooldownTag;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Cooldown")
	TSubclassOf<UGE_BaseCooldown> CooldownGameplayEffect;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ability|Cooldown")
	float CooldownDuration = 3.0f;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ability|Cooldown")
	TSubclassOf<UGameplayEffect> CooldownEffect;
	
	virtual void ActivateAbility(
		const FGameplayAbilitySpecHandle Handle,
		const FGameplayAbilityActorInfo* ActorInfo,
		const FGameplayAbilityActivationInfo ActivationInfo,
		const FGameplayEventData* TriggerEventData) override;

};

