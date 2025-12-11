// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BaseGameplayAbility.h"

#include "GA_Cast_AOE.generated.h"

/**
 * 
 */
class AGameplayAbilityTargetActor;

UCLASS()
class PROJ_API UGA_Cast_AOE : public UBaseGameplayAbility
{
	GENERATED_BODY()
	
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSubclassOf<AGameplayAbilityTargetActor> IndicatorClass;

	UPROPERTY()
	AGameplayAbilityTargetActor* Indicator;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float Range;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float Radius;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float Height;

	UPROPERTY(BlueprintReadOnly)
	FVector SpawnLocation;
	
	
protected:

	FGameplayAbilitySpecHandle CachedHandle;
	const FGameplayAbilityActorInfo* CachedActorInfo;
	FGameplayAbilityActivationInfo CachedActivationInfo;

	
	virtual  void ActivateAbility(const FGameplayAbilitySpecHandle Handle,
		const FGameplayAbilityActorInfo* ActorInfo,
		const FGameplayAbilityActivationInfo ActivationInfo,
		const FGameplayEventData* TriggerEventData) override;
	
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	void OnCancel(const FGameplayAbilityTargetDataHandle& Data);
	virtual void OnCancel_Implementation(const FGameplayAbilityTargetDataHandle& Data);

	
	
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	void OnConfirm(const FGameplayAbilityTargetDataHandle& Data);
	virtual void OnConfirm_Implementation(const FGameplayAbilityTargetDataHandle& Data);

	UFUNCTION()
	void OnTargetDataReceived(const FGameplayAbilityTargetDataHandle& Data);
	UFUNCTION()
	void OnTargetDataCancelled(const FGameplayAbilityTargetDataHandle& Data);
	
};
