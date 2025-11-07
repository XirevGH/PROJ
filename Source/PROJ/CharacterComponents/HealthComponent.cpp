// Fill out your copyright notice in the Description page of Project Settings.


#include "HealthComponent.h"
#include "AbilitySystemComponent.h"
#include "PROJ/GameplayAbilitySystem/AttributeSets/CharacterAttributeSet.h"

// Sets default values for this component's properties
UHealthComponent::UHealthComponent()
{
}

void UHealthComponent::Initialize(UAbilitySystemComponent* ASC)
{
	ASC->GetGameplayAttributeValueChangeDelegate(
		UCharacterAttributeSet::GetCurrentHealthAttribute()
	).AddUObject(this, &UHealthComponent::OnCurrentHealthChanged);
}

void UHealthComponent::OnCurrentHealthChanged(const FOnAttributeChangeData& Data) const
{
	float NewCurrentHealth = Data.NewValue;
	OnCurrentHealthChangedDelegate.Broadcast(NewCurrentHealth);
}

void UHealthComponent::TakeDamage(float Amount)
{
	UE_LOG(LogTemp, Warning, TEXT("CharacterComponent::TakeDamage %f"), Amount);
}

void UHealthComponent::Heal(float Amount)
{
	UE_LOG(LogTemp, Warning, TEXT("CharacterComponent::Heal %f"), Amount);
}

void UHealthComponent::Die()
{
	UE_LOG(LogTemp, Warning, TEXT("CharacterComponent::Die"));
}




