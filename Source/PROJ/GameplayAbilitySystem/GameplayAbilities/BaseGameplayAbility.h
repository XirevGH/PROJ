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

	 virtual void OnGiveAbility(const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilitySpec& Spec) override;
	virtual void ApplyCooldown(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo) const override;

	UFUNCTION(BlueprintCallable)
	TArray<FGameplayEffectSpecHandle> MakeEffectSpecsHandles();
	
	static FGameplayTag GetCooldownTagFromInputID(const FGameplayTag InputTag);
	virtual const FGameplayTagContainer* GetCooldownTags() const override;
	
	UPROPERTY(EditDefaultsOnly, Category = "Custom Values| Input")
	FGameplayTag InputTag;
	
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category =  "Ability")
	EAbilityInputID AbilityInputID{ EAbilityInputID::None };
	
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category="Effects")
	TArray<TSubclassOf<UGameplayEffect>> Effects;
	
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category="Effects")
	TMap<FGameplayTag, float> SetByCallerValues;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Cooldown")
	float Cooldown = 0;

	UPROPERTY()
	FGameplayTag CooldownTag;   

	UPROPERTY()
	FGameplayTagContainer CooldownTagContainer;
};
