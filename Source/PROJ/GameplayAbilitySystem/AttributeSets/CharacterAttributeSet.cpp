// Fill out your copyright notice in the Description page of Project Settings.


#include "CharacterAttributeSet.h"
#include "PROJ/Characters/BaseCharacter.h"
#include "Net/UnrealNetwork.h"

void UCharacterAttributeSet::OnRep_CurrentHealth(const FGameplayAttributeData& OldValue) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UCharacterAttributeSet, CurrentHealth, OldValue);
}

void UCharacterAttributeSet::OnRep_MaxHealth(const FGameplayAttributeData& OldValue) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UCharacterAttributeSet, MaxHealth, OldValue);
}

void UCharacterAttributeSet::OnRep_CurrentMoveSpeed(const FGameplayAttributeData& OldValue) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UCharacterAttributeSet, CurrentMoveSpeed, OldValue);
}

void UCharacterAttributeSet::OnRep_MaxMoveSpeed(const FGameplayAttributeData& OldValue) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UCharacterAttributeSet, MaxMoveSpeed, OldValue);
}


void UCharacterAttributeSet::OnRep_MaxMana(const FGameplayAttributeData& OldValue) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UCharacterAttributeSet, MaxMana, OldValue);
}

void UCharacterAttributeSet::OnRep_Mana(const FGameplayAttributeData& OldValue) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UCharacterAttributeSet, Mana, OldValue);
}
void UCharacterAttributeSet::OnRep_ConduitCharges(const FGameplayAttributeData& OldValue) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UCharacterAttributeSet, ConduitCharges, OldValue);
}

void UCharacterAttributeSet::OnRep_MaxConduitCharges(const FGameplayAttributeData& OldValue) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UCharacterAttributeSet, MaxConduitCharges, OldValue);
}
void UCharacterAttributeSet::OnRep_DamageMultiplier(const FGameplayAttributeData& OldValue) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UCharacterAttributeSet, DamageMultiplier, OldValue);
}
void UCharacterAttributeSet::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME_CONDITION_NOTIFY(UCharacterAttributeSet, CurrentHealth, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UCharacterAttributeSet, MaxHealth, COND_None, REPNOTIFY_Always);

	DOREPLIFETIME_CONDITION_NOTIFY(UCharacterAttributeSet, CurrentMoveSpeed, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UCharacterAttributeSet, MaxMoveSpeed, COND_None, REPNOTIFY_Always);
	
	DOREPLIFETIME_CONDITION_NOTIFY(UCharacterAttributeSet, Mana, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UCharacterAttributeSet, MaxMana, COND_None, REPNOTIFY_Always);

	DOREPLIFETIME_CONDITION_NOTIFY(UCharacterAttributeSet,ConduitCharges, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UCharacterAttributeSet,MaxConduitCharges, COND_None, REPNOTIFY_Always);
	
	DOREPLIFETIME_CONDITION_NOTIFY(UCharacterAttributeSet,DamageMultiplier, COND_None, REPNOTIFY_Always);
}

void UCharacterAttributeSet::PostGameplayEffectExecute(const FGameplayEffectModCallbackData& Data)
{
	Super::PostGameplayEffectExecute(Data);

	if (Data.EvaluatedData.Attribute == GetCurrentHealthAttribute())
	{
		float DamageDealt = -Data.EvaluatedData.Magnitude;

		if (DamageDealt > 0.0f)
		{
			AActor* TargetActor = nullptr;
			if (Data.Target.AbilityActorInfo.IsValid() && Data.Target.AbilityActorInfo->AvatarActor.IsValid())
			{
				TargetActor = Data.Target.AbilityActorInfo->AvatarActor.Get();
			}
			
			AActor* SourceActor = Data.EffectSpec.GetContext().GetInstigator();
			ABaseCharacter* AttackerChar = nullptr;
			
			if (SourceActor)
			{
				if (APlayerState* PS = Cast<APlayerState>(SourceActor))
				{
					AttackerChar = Cast<ABaseCharacter>(PS->GetPawn());
				}
				else if (AController* Controller = Cast<AController>(SourceActor))
				{
					AttackerChar = Cast<ABaseCharacter>(Controller->GetPawn());
				}
				else
				{
					AttackerChar = Cast<ABaseCharacter>(SourceActor);
				}
			}

			if (AttackerChar)
			{
				bool bIsCrit = GetIncomingCritFlag() > 0.0f;
				
				AttackerChar->HandleDamageDealt(TargetActor, DamageDealt, bIsCrit);

				SetIncomingCritFlag(0.0f);
			}
		}
		
		SetCurrentHealth(FMath::Clamp(GetCurrentHealth(), 0.f, GetMaxHealth()));
	}
}