// Fill out your copyright notice in the Description page of Project Settings.


#include "GEEC_Character.h"

#include "PROJ/GameplayAbilitySystem/BaseAbilitySystemComponent.h"
#include "PROJ/GameplayAbilitySystem/AttributeSets/CharacterAttributeSet.h"

struct FDamageStatics
{
	DECLARE_ATTRIBUTE_CAPTUREDEF(CurrentHealth)

	FDamageStatics()
	{
		// Capture target's health
		DEFINE_ATTRIBUTE_CAPTUREDEF(UCharacterAttributeSet, CurrentHealth, Target, false);
	}
};

static FDamageStatics& DamageStatics()
{
	static FDamageStatics Statics;
	return Statics;
}

UGEEC_Character::UGEEC_Character()
{
	// Register attributes we want to capture
	RelevantAttributesToCapture.Add(DamageStatics().CurrentHealthDef);
}

void UGEEC_Character::Execute_Implementation(const FGameplayEffectCustomExecutionParameters& ExecutionParams,
                                             FGameplayEffectCustomExecutionOutput& OutExecutionOutput) const
{
    Super::Execute_Implementation(ExecutionParams, OutExecutionOutput);
	
    UBaseAbilitySystemComponent* TargetASC = Cast<UBaseAbilitySystemComponent>(ExecutionParams.GetTargetAbilitySystemComponent());
    const UBaseAbilitySystemComponent* SourceASC = Cast<UBaseAbilitySystemComponent>(ExecutionParams.GetSourceAbilitySystemComponent());

    if (!TargetASC || !SourceASC)
    {
        UE_LOG(LogTemp, Warning, TEXT("Invalid ASC in Damage Calculation"));
        return;
    }

    const FGameplayEffectSpec& Spec = ExecutionParams.GetOwningSpec();
    
    const FGameplayTagContainer* SourceTags = Spec.CapturedSourceTags.GetAggregatedTags();
    const FGameplayTagContainer* TargetTags = Spec.CapturedTargetTags.GetAggregatedTags();

    FAggregatorEvaluateParameters EvaluateParameters;
    EvaluateParameters.SourceTags = SourceTags;
    EvaluateParameters.TargetTags = TargetTags;

    // Get damage from weapon
    float Damage = 0.0f;
	Damage = Spec.GetSetByCallerMagnitude(FGameplayTag::RequestGameplayTag(FName("Data.Damage")), true);
    // First try to get damage from captured attribute
    float Health = 0.0f;
    ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(DamageStatics().CurrentHealthDef, EvaluateParameters, Health);

    if (Health <= 0.0f)
    {
       return;
    }
    
    // You can add modifiers here (critical hits, buffs, etc.)
    float FinalDamage = Damage * FMath::FRandRange(0.95f, 1.05);

	UE_LOG(LogTemp, Warning, TEXT("Deal Damage by %f"), FinalDamage);
    // Apply the damage as negative health
    if (FinalDamage > 0.0f)
    {
        OutExecutionOutput.AddOutputModifier(FGameplayModifierEvaluatedData(DamageStatics().CurrentHealthProperty, 
                                                                         EGameplayModOp::Additive, 
                                                                         -FinalDamage));
    }
}
