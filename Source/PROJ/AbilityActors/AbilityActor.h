// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "AbilityActor.generated.h"

struct FGameplayEffectSpecHandle;
class UGameplayEffect;
class UBaseGameplayAbility;


UCLASS()
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
	class UAbilitySystemComponent* CasterASC;
	
	UPROPERTY(BlueprintReadOnly)
	UBaseGameplayAbility *CastedAbility;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="Effects")
	TArray<FGameplayEffectSpecHandle> EffectSpecHandles;


protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	virtual void Cleanup();

	UFUNCTION(BlueprintCallable)
	bool ApplyEffectToTarget(const AActor* Target);
	
public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;
};
