// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "NiagaraFunctionLibrary.h"
#include "BaseGameplayAbility.h"
#include "HeroicSlam.generated.h"

class AGameplayAbilityTargetActor;
class ABaseCharacter;
class UNiagaraSystem;
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
	TSubclassOf<AGameplayAbilityTargetActor> IndicatorClass;

	UPROPERTY()
	AGameplayAbilityTargetActor* Indicator;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float Radius;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float Height;
	
	UPROPERTY(EditDefaultsOnly, Category="Slam")
	UNiagaraSystem* SlamVfx;

	UPROPERTY(EditDefaultsOnly, Category="Slam")
	TSubclassOf<UCameraShakeBase> SlamCameraShake;

	UPROPERTY(EditDefaultsOnly, Category="Slam")
	float SlamRadius = 800.f;
	UPROPERTY()
	bool bLeapEffectsApplied = false;
	/*Attributes for air and launch arc*/
	UPROPERTY()
	float OriginalAirControl;
	UPROPERTY()
	float OriginalBraking;
	UPROPERTY()
	float OriginalFriction;

	UFUNCTION()
	void RestoreAirFriction();
	UFUNCTION(BlueprintCallable)
	void LaunchToTarget();
	UFUNCTION(BlueprintCallable)
	void OnConfirm(const FGameplayAbilityTargetDataHandle& Data);
	UFUNCTION(BlueprintCallable)
	void OnCancel(const FGameplayAbilityTargetDataHandle& Data);
	UFUNCTION()
	void ApplyEffectsToTarget(AActor* Target);
	
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

	UPROPERTY()
	float CachedOriginalMaxSpeed = 600.f;

	UPROPERTY()
	ABaseCharacter* CachedPlayer;

	FTimerHandle LandingCheckTimer;

	UFUNCTION()
	void LandingCheck();
};
