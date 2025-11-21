// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilityActor.h"
#include "AbilitySystemComponent.h"
#include "AbilitySystemGlobals.h"
#include "./PROJ/GameplayAbilitySystem/GameplayAbilities/BaseGameplayAbility.h"


// Sets default values
AAbilityActor::AAbilityActor()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
}

// Called when the game starts or when spawned
void AAbilityActor::BeginPlay()
{
	Super::BeginPlay();
	
}

void AAbilityActor::Cleanup()
{
}

// Called every frame
void AAbilityActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

bool AAbilityActor::ApplyEffectToTarget(const AActor* Target)
{
	if (!HasAuthority())
		return false;

	if (!CasterASC)
	{
		UE_LOG(LogTemp, Error, TEXT("Caster ASC is NULL"));
		return false;
	}

	if (!CastedAbility)
	{
		UE_LOG(LogTemp, Error, TEXT("CastedAbility is NULL"));
		return false;
	}

	UAbilitySystemComponent* TargetASC =
		UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(Target);

	if (!TargetASC)
	{
		UE_LOG(LogTemp, Error, TEXT("Target ASC is NULL! %s has no ASC"), *GetNameSafe(Target));
		return false;
	}

	
	//UE_LOG(LogTemp, Warning, TEXT("Target ASC is from: %s"), *TargetASC->GetAvatarActor()->GetName());
	

	
	for (auto& SpecHandle : EffectSpecHandles)
	{
		if (!SpecHandle.Data.Get())
		{
			continue;
		}
		UE_LOG(LogTemp, Warning, TEXT("Applied: %s to %s"),*SpecHandle.Data->GetContext().ToString(),  *TargetASC->GetAvatarActor()->GetName());
		TargetASC->ApplyGameplayEffectSpecToSelf(*SpecHandle.Data.Get());
	}
	
	return true;
}
