#include "GEEC_Character.h"
#include "PROJ/GameplayAbilitySystem/BaseAbilitySystemComponent.h"
#include "PROJ/GameplayAbilitySystem/AttributeSets/CharacterAttributeSet.h"

struct FDamageStatics
{
	DECLARE_ATTRIBUTE_CAPTUREDEF(CurrentHealth)
	DECLARE_ATTRIBUTE_CAPTUREDEF(IncomingCritFlag)

	FDamageStatics()
	{
		DEFINE_ATTRIBUTE_CAPTUREDEF(UCharacterAttributeSet, CurrentHealth, Target, false);
		DEFINE_ATTRIBUTE_CAPTUREDEF(UCharacterAttributeSet, IncomingCritFlag, Target, false);
	}
};

static FDamageStatics& DamageStatics()
{
	static FDamageStatics Statics;
	return Statics;
}

UGEEC_Character::UGEEC_Character()
{
	RelevantAttributesToCapture.Add(DamageStatics().CurrentHealthDef);
    RelevantAttributesToCapture.Add(DamageStatics().IncomingCritFlagDef);
}

void UGEEC_Character::Execute_Implementation(const FGameplayEffectCustomExecutionParameters& ExecutionParams,
                                             FGameplayEffectCustomExecutionOutput& OutExecutionOutput) const
{
    UBaseAbilitySystemComponent* TargetASC = Cast<UBaseAbilitySystemComponent>(ExecutionParams.GetTargetAbilitySystemComponent());
    const UBaseAbilitySystemComponent* SourceASC = Cast<UBaseAbilitySystemComponent>(ExecutionParams.GetSourceAbilitySystemComponent());

    if (!TargetASC || !SourceASC)
    {
        UE_LOG(LogTemp, Warning, TEXT("Invalid ASC in Damage Calculation"));
        return;
    }

    const FGameplayEffectSpec& Spec = ExecutionParams.GetOwningSpec();
    
    FAggregatorEvaluateParameters EvaluateParameters;
    EvaluateParameters.SourceTags = Spec.CapturedSourceTags.GetAggregatedTags();
    EvaluateParameters.TargetTags = Spec.CapturedTargetTags.GetAggregatedTags();

    float Damage = Spec.GetSetByCallerMagnitude(FGameplayTag::RequestGameplayTag(FName("Data.Damage")), true);

    float Health = 0.0f;
    ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(DamageStatics().CurrentHealthDef, EvaluateParameters, Health);

    bool bIsCritical = false;
    if (FMath::RandRange(0.0f, 1.0f) <= 0.2f) 
    {
        bIsCritical = true;
        Damage *= 2.0f;
    }

    float FinalDamage = Damage * FMath::FRandRange(0.95f, 1.05);
	
	UE_LOG(LogTemp, Warning, TEXT("Deal Damage by %f"), FinalDamage);

    if (FinalDamage > 0.0f)
    {
    	if (bIsCritical)
    	{
    		OutExecutionOutput.AddOutputModifier(FGameplayModifierEvaluatedData(
				DamageStatics().IncomingCritFlagProperty,
				EGameplayModOp::Override,
				1.0f));
    	}
    	else 
    	{
    		OutExecutionOutput.AddOutputModifier(FGameplayModifierEvaluatedData(
			   DamageStatics().IncomingCritFlagProperty,
			   EGameplayModOp::Override, 
			   0.0f));
    	}
    	
    	OutExecutionOutput.AddOutputModifier(FGameplayModifierEvaluatedData(
			DamageStatics().CurrentHealthProperty, 
			EGameplayModOp::Additive, 
			-FinalDamage));
    }
}