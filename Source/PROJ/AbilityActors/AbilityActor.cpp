// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilityActor.h"
#include "AbilitySystemComponent.h"
#include "AbilitySystemGlobals.h"
#include "PROJ/Characters/BaseCharacter.h"
#include "PROJ/Framework/BasePlayerController.h"
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

bool AAbilityActor::ShouldSkipHit_Implementation(AActor* OtherActor)
{
	if (Caster)
	{
		if (ABasePlayerController* PC = Cast<ABasePlayerController>(Cast<ABaseCharacter>(Caster)->GetController()))
		{
			if (ABaseCharacter* OtherCharacter = Cast<ABaseCharacter>(OtherActor))
			{
				if (ABasePlayerController* OtherPC = Cast<ABasePlayerController>(OtherCharacter->GetController()))
				{
					return PC->TeamID.Equals(OtherPC->TeamID);
				}
			}
		}
	}

	return OtherActor == Caster || OtherActor->IsA(StaticClass());
}


bool AAbilityActor::InitializeAbilityActor(
	AActor* InCaster, 
	UAbilitySystemComponent* InCasterASC,
	UBaseGameplayAbility* InCastedAbility,
	const TArray<FGameplayEffectSpecHandle>& InEffectSpecHandles)
{
	if (!InCaster || !InCasterASC || !InCastedAbility)
	{
		UE_LOG(LogTemp, Warning, TEXT("InitializeAbilityActor failed: invalid input"));
		return false;
	}

	Caster = InCaster;
	CasterASC = InCasterASC;
	CastedAbility = InCastedAbility;
	EffectSpecHandles = InEffectSpecHandles;
    
	return true;
}

