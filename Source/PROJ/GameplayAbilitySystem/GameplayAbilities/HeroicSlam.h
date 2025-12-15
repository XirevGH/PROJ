// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "NiagaraFunctionLibrary.h"
#include "BaseGameplayAbility.h"
#include "HeroicSlam.generated.h"

class AGameplayAbilityTargetActor;
class ABaseCharacter;
class UNiagaraSystem;
class AGameplayCueNotify_Actor;
/**
 * 
 */
UCLASS()
class PROJ_API UHeroicSlam : public UBaseGameplayAbility
{
	GENERATED_BODY()

public:

	UHeroicSlam();
	
	UPROPERTY(EditDefaultsOnly, Category="Slam")
	UNiagaraSystem* SlamVfx;

	UPROPERTY(EditDefaultsOnly, Category="Slam")
	TSubclassOf<UCameraShakeBase> SlamCameraShake;

	UPROPERTY(EditDefaultsOnly, Category="Slam")
	float SlamRadius = 800.f;
	UPROPERTY(EditDefaultsOnly, Category="Slam")
	float JumpDistance = 800.f;
	UPROPERTY(EditDefaultsOnly, Category="Slam")
	float ArcParam = .5f;
	UPROPERTY()
	bool bLeapEffectsApplied = false;
	/*Attributes for air and launch arc*/
	UPROPERTY()
	float OriginalAirControl;
	UPROPERTY()
	float OriginalBraking;
	UPROPERTY()
	float OriginalFriction;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="VFX")
	TSubclassOf<AGameplayCueNotify_Actor> SlamGameplayCue;
	
	UFUNCTION()
	void RestoreAirFriction();
	UFUNCTION(BlueprintCallable)
	void LaunchToTarget();
	
	virtual void ActivateAbility(
		const FGameplayAbilitySpecHandle Handle,
		const FGameplayAbilityActorInfo* ActorInfo,
		const FGameplayAbilityActivationInfo ActivationInfo,
		const FGameplayEventData* TriggerEventData) override;

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
