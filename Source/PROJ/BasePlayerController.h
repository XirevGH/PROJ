// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemInterface.h"
#include "GameplayTagContainer.h"
#include "GameFramework/PlayerController.h"
#include "BasePlayerController.generated.h"

class UBaseAbilitySystemComponent;
class UBaseInputConfig;
/**
 * 
 */
UCLASS()
class PROJ_API ABasePlayerController : public APlayerController, public IAbilitySystemInterface
{
	GENERATED_BODY()

public:

	virtual void SetupInputComponent() override;
	
	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;

protected:

	virtual void BeginPlay() override;

	void AbilityInputPressed(FGameplayTag InputTag);
	void AbilityInputReleased(FGameplayTag InputTag);
private:

	UPROPERTY()
	mutable TObjectPtr<UBaseAbilitySystemComponent> BaseAbilitySystemComponent = nullptr;
	
	UPROPERTY(EditDefaultsOnly, Category = "Custom Values|Input")
	TObjectPtr<UBaseInputConfig> BaseInputConfig;
};
