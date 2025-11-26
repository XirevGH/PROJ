// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BaseGameplayAbility.h"
#include "ConeBlast.generated.h"

class UNiagaraSystem;
class ABaseCharacter;
/**
 * 
 */
UCLASS()
class PROJ_API UConeBlast : public UBaseGameplayAbility
{
	GENERATED_BODY()
	
public:
	UConeBlast();
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Montage")
	FGameplayTag MontageNotifyTag;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="VFX")
	UNiagaraSystem* ConeBlastVFX;

	UFUNCTION()
	void OnMontageNotifyReceived(FGameplayEventData Payload);
	
	virtual void ActivateAbility(
		const FGameplayAbilitySpecHandle Handle,
		const FGameplayAbilityActorInfo* ActorInfo,
		const FGameplayAbilityActivationInfo ActivationInfo,
		const FGameplayEventData* TriggerEventData) override;

	
	virtual void PlayMontage(UAnimMontage* Montage) override;
	
	virtual void OnMontageCompleted() override;

protected:
	/*Cone Setting*/
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Cone Settings")
	float ConeRange = 800.f;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Cone Settings")
	float ConeHalfAngle = 45.f;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Cone Settings")
	float ConeHeight = 200.f;

	/*Debug setting*/
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Cone Settings|Debug")
	int32 DebugLines = 32;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Cone Settings|Debug")
	float DebugDuration = 4.f;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Cone Settings|Debug")
	bool bShowDebug = true;

	
private:
	UFUNCTION(BlueprintCallable, Category = "Cone Execute")
	void ExecuteConeAttack();

	bool IsActorInCone(const FVector& Origin, const FVector& ForwardVector, AActor* Target) const;

	void DrawConeMesh(const FVector& Origin, const FVector& ForwardVector)const;

	UPROPERTY()
	ABaseCharacter* CachedPlayer;
	
};
