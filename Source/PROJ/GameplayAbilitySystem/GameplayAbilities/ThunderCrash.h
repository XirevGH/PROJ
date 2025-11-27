// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BaseGameplayAbility.h"
#include "ThunderCrash.generated.h"

class ABaseCharacter;
/**
 * 
 */
UCLASS()
class PROJ_API UThunderCrash : public UBaseGameplayAbility
{
	GENERATED_BODY()
public:
	UThunderCrash();
	void MakeMontageWaitEvent();

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Montage")
	FGameplayTag MontageNotifyTag;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="ConduitActor")
	TSubclassOf<AAbilityActor> ConduitActor;
	
	virtual void PlayMontage(UAnimMontage* Montage) override;
	
	UFUNCTION()
	void OnMontageNotifyReceived(FGameplayEventData Payload);
	
	virtual void ActivateAbility(
		const FGameplayAbilitySpecHandle Handle,
		const FGameplayAbilityActorInfo* ActorInfo,
		const FGameplayAbilityActivationInfo ActivationInfo,
		const FGameplayEventData* TriggerEventData) override;

	void SpawnConduit();
private:
	
	UPROPERTY()
	ABaseCharacter* CachedPlayer;
};
