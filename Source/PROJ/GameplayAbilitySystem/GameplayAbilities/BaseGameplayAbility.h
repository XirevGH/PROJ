// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbility.h"
#include "./PROJ/PROJ.h"
#include "BaseGameplayAbility.generated.h"

/**
 * 
 */
UCLASS()
class PROJ_API UBaseGameplayAbility : public UGameplayAbility
{
	GENERATED_BODY()
public:

	UPROPERTY(EditDefaultsOnly, Category = "Custom Values| Input")
	FGameplayTag InputTag;
	
	
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category =  "Ability")
	EAbilityInputID AbilityInputID{ EAbilityInputID::None };

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category="Effects")
	TArray<TSubclassOf<UGameplayEffect>> DefaultEffects;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Damage")
	float BaseDamage = 0;
};
