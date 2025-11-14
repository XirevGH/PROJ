// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BaseGameplayAbility.h"
#include "GA_Cast_AOE.generated.h"

/**
 * 
 */
UCLASS()
class PROJ_API UGA_Cast_AOE : public UBaseGameplayAbility
{
	GENERATED_BODY()

protected:
	
	virtual  void ActivateAbility(const FGameplayAbilitySpecHandle Handle,
		const FGameplayAbilityActorInfo* ActorInfo,
		const FGameplayAbilityActivationInfo ActivationInfo,
		const FGameplayEventData* TriggerEventData) override;

	UPROPERTY(EditAnywhere,BlueprintReadWrite)
	float CollisionRadius;

};
