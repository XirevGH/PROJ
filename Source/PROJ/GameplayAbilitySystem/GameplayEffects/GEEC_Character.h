// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayEffectExecutionCalculation.h"
#include "GEEC_Character.generated.h"

/**
 * 
 */
UCLASS()
class PROJ_API UGEEC_Character : public UGameplayEffectExecutionCalculation
{
	GENERATED_BODY()
public:
	UGEEC_Character();

protected:
	virtual  void Execute_Implementation(
		const FGameplayEffectCustomExecutionParameters& ExecutionParams,
		FGameplayEffectCustomExecutionOutput& OutExecutionOutput) const override;
	
};
