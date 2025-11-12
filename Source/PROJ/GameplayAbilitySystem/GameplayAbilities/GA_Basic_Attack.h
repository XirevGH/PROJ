// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BaseGameplayAbility.h"
#include "GA_Basic_Attack.generated.h"

class UAbilityTask_PlayMontageAndWait;
/**
 * 
 */
UCLASS()
class PROJ_API UGA_Basic_Attack : public UBaseGameplayAbility
{
	GENERATED_BODY()

	public:
	UGA_Basic_Attack();
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Ability|Animation")
	UAnimMontage* MyMontage;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	AActor* WeaponActor;

protected:
	virtual  void ActivateAbility(const FGameplayAbilitySpecHandle Handle,
		const FGameplayAbilityActorInfo* ActorInfo,
		const FGameplayAbilityActivationInfo ActivationInfo,
		const FGameplayEventData* TriggerEventData) override;
	

	UFUNCTION()
	void OnHitscanStart();

	UFUNCTION()
	void OnHitscanEnd();
	
	UFUNCTION(BlueprintImplementableEvent)
	void OntargetReady(const FGameplayAbilityTargetDataHandle& TargetData);

	UFUNCTION(BlueprintCallable)
	void Attack();

	UFUNCTION()
	void OnMontageCompleted();

	UFUNCTION()
	void OnMontageInterrupted();

	UFUNCTION()
	void OnMontageCancelled();
};
