// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "HealthComponent.generated.h"

struct FOnAttributeChangeData;
class UAbilitySystemComponent;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnCurrentHealthChangedDelegate, float, CurrentHealth);

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class PROJ_API UHealthComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	// Sets default values for this component's properties
	UHealthComponent();

protected:
	// Called when the game starts
	void Initialize(UAbilitySystemComponent* ASC);

	private:
	void OnCurrentHealthChanged(const FOnAttributeChangeData& Data) const;
	
	UPROPERTY(BlueprintAssignable, Category = "Gameplay")
	FOnCurrentHealthChangedDelegate OnCurrentHealthChangedDelegate;

	void TakeDamage(float Amount);
	void Heal(float Amount);
	void Die();
	
	
};
