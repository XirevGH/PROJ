// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "AbilitySystemInterface.h"

#include "BasePlayerState.generated.h"

class UBaseAbilitySystemComponent;
class UCharacterAttributeSet;
class UAttributeSet;
class UGameplayAbility;

/**
 * 
 */
UCLASS()
class PROJ_API ABasePlayerState : public APlayerState, public IAbilitySystemInterface
{
	GENERATED_BODY()

public:
	ABasePlayerState();
	
	virtual void BeginPlay() override;
	
	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;
	
	UFUNCTION(BlueprintPure)
	UBaseAbilitySystemComponent* GetBaseAbilitySystemComponent() const;
	
	UFUNCTION(BlueprintPure)
	UCharacterAttributeSet* GetCharacterAttributeSet() const;

	UFUNCTION()
	void GiveDefaultAbilities(const TArray<TSubclassOf<UGameplayAbility>>& Abilities);

	void InitializeASC(class AActor* Avatar);

	
private:
	
	UPROPERTY(BlueprintReadOnly, meta=(AllowPrivateAccess))
	TObjectPtr<UBaseAbilitySystemComponent> BaseAbilitySystemComponent;

	UPROPERTY(Transient)
	UCharacterAttributeSet* BaseAttributeSet;
	
};
