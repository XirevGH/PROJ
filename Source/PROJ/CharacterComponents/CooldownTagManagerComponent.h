// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "GameplayEffectTypes.h"
#include "Components/ActorComponent.h"
#include "CooldownTagManagerComponent.generated.h"


class UAbilitySystemComponent;
struct FActiveGameplayEffect;
struct FGameplayEffectSpec;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class PROJ_API UCooldownTagManagerComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	UCooldownTagManagerComponent();

	void Initialize(UAbilitySystemComponent* InASC);
	
protected:

	UPROPERTY()
	UAbilitySystemComponent* ASC;

	TMap<FGameplayTag, int32> ActiveCooldownCounts;

	UFUNCTION()
	void OnActiveGEAdded(
		UAbilitySystemComponent* TargetASC,
		const FGameplayEffectSpec& Spec,
		FActiveGameplayEffectHandle Handle);

	UFUNCTION()
	void OnActiveGERemoved(const FActiveGameplayEffect& ActiveGE);
};
