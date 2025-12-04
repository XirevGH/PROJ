// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "PROJ/GameplayAbilitySystem/GameplayAbilities/BaseGameplayAbility.h"
#include "AbilityActor.generated.h"

struct FGameplayEffectSpecHandle;
class UGameplayEffect;
class UBaseGameplayAbility;


UCLASS(Blueprintable)
class PROJ_API AAbilityActor : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	AAbilityActor();

	// The actor that fired this projectile
	UPROPERTY(BlueprintReadOnly)
	AActor* Caster;

	// The caster's AbilitySystemComponent
	UPROPERTY(BlueprintReadOnly)
	UAbilitySystemComponent* CasterASC;
	
	UPROPERTY(BlueprintReadOnly)
	FAbilityEffectSpecs EffectSpecHandles;
	
	UFUNCTION(BlueprintCallable)
	bool InitializeAbilityActor(AActor* InCaster, UAbilitySystemComponent* InCasterASC,
		const FAbilityEffectSpecs& InEffectSpecHandles);

private:
	//should not call this in bp or anywhere else but inside ApplyEffectToTarget(AActor* Target)
	void ApplySpecArrayToASC(const TArray<FGameplayEffectSpecHandle>& Specs, UAbilitySystemComponent* ASC);
	
protected:
	virtual void BeginPlay() override;
	virtual void Cleanup();

	UFUNCTION(BlueprintCallable)
	void ApplyEffectToTarget(AActor* Target);

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	bool ShouldSkipHit(AActor* OtherActor);

	bool ShouldSkipHit_Implementation(AActor* OtherActor);
	
public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;
};
