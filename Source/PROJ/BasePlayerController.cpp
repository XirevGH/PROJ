// Fill out your copyright notice in the Description page of Project Settings.


#include "BasePlayerController.h"

#include "GameplayAbilitySystem/BaseAbilitySystemComponent.h"
#include "GameplayAbilitySystem/BasePlayerState.h"
#include "Input/BaseEnhancedInputComponent.h"

void ABasePlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();

	if (UBaseEnhancedInputComponent* BaseInputComp = Cast<UBaseEnhancedInputComponent>(InputComponent))
	{
		BaseInputComp->BindAbilityActions(BaseInputConfig, this, &ThisClass::AbilityInputPressed, &ThisClass::AbilityInputReleased);
	}
}

UAbilitySystemComponent* ABasePlayerController::GetAbilitySystemComponent() const
{
	return BaseAbilitySystemComponent;
}

void ABasePlayerController::BeginPlay()
{
	Super::BeginPlay();
	
	if (const ABasePlayerState* BasePlayerState = GetPlayerState<ABasePlayerState>())
	{
		BaseAbilitySystemComponent = BasePlayerState->GetBaseAbilitySystemComponent();
	}
}

void ABasePlayerController::AbilityInputPressed(FGameplayTag InputTag)
{
	if (IsValid(BaseAbilitySystemComponent))
	{
		BaseAbilitySystemComponent->AbilityInputPressed(InputTag);
	}
}

void ABasePlayerController::AbilityInputReleased(FGameplayTag InputTag)
{
	if (IsValid(BaseAbilitySystemComponent))
	{
		BaseAbilitySystemComponent->AbilityInputReleased(InputTag);
	}
}
