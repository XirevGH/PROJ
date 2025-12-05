// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BaseGameplayAbility.h"
#include "BaseAttack.generated.h"

class UAbilityData;
class UAbilityTask_WaitGameplayEvent;
class AWeapon;
class UAbilityTask_PlayMontageAndWait;
/**
 * 
 */
UCLASS(Blueprintable)
class PROJ_API UBaseAttack : public UBaseGameplayAbility
{
	GENERATED_BODY()

	public:
	UBaseAttack();
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Ability|Animation")
	UAnimMontage* MyMontage;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FName WeaponStartSocket = "WeaponStartSocket";

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FName WeaponEndSocket = "WeaponEndSocket";
	
	UPROPERTY()
	TArray<AActor*> Targets;

	FTimerHandle HitScanTimerHandle;
	
	bool bIsHitscanActive = false;

	UPROPERTY()
	UAbilityTask_WaitGameplayEvent* StartTask;

	UPROPERTY()
	UAbilityTask_WaitGameplayEvent* EndTask;
	
protected:
	

	virtual void OnGiveAbility(const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilitySpec& Spec) override;
	
	virtual  void ActivateAbility(
		const FGameplayAbilitySpecHandle Handle,
	    const FGameplayAbilityActorInfo* ActorInfo,
	    const FGameplayAbilityActivationInfo ActivationInfo,
	    const FGameplayEventData* TriggerEventData) override;
	
	virtual void EndAbility(const FGameplayAbilitySpecHandle Handle,
		const FGameplayAbilityActorInfo* ActorInfo,
		const FGameplayAbilityActivationInfo ActivationInfo,
		bool bReplicatedEndAbility,
		bool bWasCancelled) override;
	
	void SetupHitScanTasks();
	void ClearExistingTasks();
	
	/*Events from HitScan*/
	void StartHitScan();
	void EndHitScan();
	void PerformHitScan();
	
	UFUNCTION(Server, Reliable)
	void Server_HitScanStart();
	UFUNCTION(Server, Reliable)
	void Server_EndHitScan();

	FVector GetSocketLocation(const FName& SocketName) const;
	
	UFUNCTION()
	void OnHitscanStart(FGameplayEventData Payload);

	UFUNCTION()
	void OnHitscanEnd(FGameplayEventData Payload);
	
};
