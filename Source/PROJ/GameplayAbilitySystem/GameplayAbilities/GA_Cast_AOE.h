// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BaseGameplayAbility.h"

#include "GA_Cast_AOE.generated.h"

/**
 * 
 */
class AIndicator;

UCLASS()
class PROJ_API UGA_Cast_AOE : public UBaseGameplayAbility
{
	GENERATED_BODY()
	
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSubclassOf<AIndicator> IndicatorClass;

	UPROPERTY()
	AIndicator* Indicator;
	
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
	void OnCancel();
	virtual void OnCancel_Implementation();

	
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	void OnConfirm();
	virtual void OnConfirm_Implementation();

	UFUNCTION(Server, Reliable)
	void ServerConfirmTarget(const FVector& Location);
	void ServerConfirmTarget_Implementation(const FVector& Location);
	
};
