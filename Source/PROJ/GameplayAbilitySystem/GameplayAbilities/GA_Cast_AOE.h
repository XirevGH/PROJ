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
	
public:
	UPROPERTY(EditAnywhere,BlueprintReadWrite)
	TSubclassOf<class AGameplayAbilityTargetActor> IndicatorActorClass;


	UPROPERTY(EditAnywhere,BlueprintReadWrite)
	float Range;

	UPROPERTY(EditAnywhere,BlueprintReadWrite)
	float Radius;

	UPROPERTY(EditAnywhere,BlueprintReadWrite)
	float Height;
	
protected:
	
	virtual  void ActivateAbility(const FGameplayAbilitySpecHandle Handle,
		const FGameplayAbilityActorInfo* ActorInfo,
		const FGameplayAbilityActivationInfo ActivationInfo,
		const FGameplayEventData* TriggerEventData) override;

	UFUNCTION()
	void OnTargetDataReceived(const FGameplayAbilityTargetDataHandle& Data);
	UFUNCTION()
	void OnTargetDataCancelled(const FGameplayAbilityTargetDataHandle& Data);
};
