

#include "AbilityActor.h"
#include "AbilitySystemComponent.h"
#include "AbilitySystemGlobals.h"
#include "PROJ/Characters/BaseCharacter.h"
#include "PROJ/Framework/BasePlayerController.h"
#include "./PROJ/GameplayAbilitySystem/GameplayAbilities/BaseGameplayAbility.h"


AAbilityActor::AAbilityActor()
{
	PrimaryActorTick.bCanEverTick = true;
}

void AAbilityActor::BeginPlay()
{
	Super::BeginPlay();
}

void AAbilityActor::Cleanup()
{
}
void AAbilityActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

bool AAbilityActor::ApplyEffectToTarget(AActor* Target)
{
	if (!HasAuthority() || !CasterASC || !CastedAbility)
	{
		UE_LOG(LogTemp, Error, TEXT("No Auhtory OR Caster ASC OR CastedAbility is NULL"));
		return false;
	}
	
	UAbilitySystemComponent* TargetASC =
		UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(Target);
	if (!TargetASC)
	{
		UE_LOG(LogTemp, Error, TEXT("Target ASC is NULL! %s has no ASC"), *GetNameSafe(Target));
		return false;
	}

	for (auto& SpecHandle : EffectSpecHandles)
	{
		if (!SpecHandle.IsValid()) continue;
		
		TargetASC->ApplyGameplayEffectSpecToSelf(*SpecHandle.Data.Get());
		UE_LOG(LogTemp, Warning, TEXT("Applied: %s to %s"),*SpecHandle.Data->GetContext().ToString(),  *TargetASC->GetAvatarActor()->GetName());
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

