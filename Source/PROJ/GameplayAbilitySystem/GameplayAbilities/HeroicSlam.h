// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BaseGameplayAbility.h"
#include "HeroicSlam.generated.h"

class AIndicator;
/**
 * 
 */
UCLASS()
class PROJ_API UHeroicSlam : public UBaseGameplayAbility
{
	GENERATED_BODY()

public:

	UHeroicSlam();


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

	UFUNCTION()
	void LaunchToTarget();
	UFUNCTION(BlueprintCallable)
	void OnConfirm();
	UFUNCTION(BlueprintCallable)
	void OnCancel();
	
	virtual void ActivateAbility(
		const FGameplayAbilitySpecHandle Handle,
		const FGameplayAbilityActorInfo* ActorInfo,
		const FGameplayAbilityActivationInfo ActivationInfo,
		const FGameplayEventData* TriggerEventData) override;

	virtual void EndAbility(
		const FGameplayAbilitySpecHandle Handle,
		const FGameplayAbilityActorInfo* ActorInfo,
	    const FGameplayAbilityActivationInfo ActivationInfo,
	    bool bReplicatedEndAbility, bool bWasCancelled) override;
private:
	UPROPERTY()
	FVector TargetLocation; 
};
