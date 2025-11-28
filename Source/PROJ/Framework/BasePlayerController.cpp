// Fill out your copyright notice in the Description page of Project Settings.


#include "BasePlayerController.h"
#include "../GameplayAbilitySystem/BaseAbilitySystemComponent.h"
#include "BasePlayerState.h"
#include "../Input/BaseEnhancedInputComponent.h"
#include "Net/UnrealNetwork.h"

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
	if (!IsValid(BaseAbilitySystemComponent))
	{
		if (const ABasePlayerState* PS = GetPlayerState<ABasePlayerState>())
		{
			BaseAbilitySystemComponent = PS->GetBaseAbilitySystemComponent();
		}
	}
	return BaseAbilitySystemComponent;
}

void ABasePlayerController::BeginPlay()
{
	Super::BeginPlay();
}

void ABasePlayerController::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	// This command is what actually makes the engine send the data
	DOREPLIFETIME(ABasePlayerController, TeamID);
}

void ABasePlayerController::AbilityInputPressed(FGameplayTag InputTag)
{
	if (IsValid(GetAbilitySystemComponent()))
	{
		BaseAbilitySystemComponent->AbilityInputPressed(InputTag);
	}
}

void ABasePlayerController::AbilityInputReleased(FGameplayTag InputTag)
{
	if (IsValid(GetAbilitySystemComponent()))
	{
		BaseAbilitySystemComponent->AbilityInputReleased(InputTag);
	}
}

void ABasePlayerController::OnRep_PlayerState()
{
	Super::OnRep_PlayerState();
	OnPlayerStateReady();
}
