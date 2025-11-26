// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BaseGameplayAbility.h"
#include "GA_Interact.generated.h"

/**
 * 
 */
UCLASS()
class PROJ_API UGA_Interact : public UBaseGameplayAbility
{
	GENERATED_BODY()
public:
	
	virtual  void ActivateAbility(const FGameplayAbilitySpecHandle Handle,
		const FGameplayAbilityActorInfo* ActorInfo,
		const FGameplayAbilityActivationInfo ActivationInfo,
		const FGameplayEventData* TriggerEventData) override;
	
	void OnGiveAbility(const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilitySpec& Spec);

	UFUNCTION(Server, Reliable)
	void Server_Interact(AActor* Target, UActorComponent* Component);
	void Server_Interact_Implementation(AActor* Target, UActorComponent* Component);
};
