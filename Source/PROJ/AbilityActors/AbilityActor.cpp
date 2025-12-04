

#include "AbilityActor.h"
#include "AbilitySystemComponent.h"
#include "AbilitySystemGlobals.h"
#include "PROJ/Characters/BaseCharacter.h"
#include "PROJ/Framework/BasePlayerController.h"
#include "./PROJ/GameplayAbilitySystem/GameplayAbilities/BaseGameplayAbility.h"


AAbilityActor::AAbilityActor()
{
	PrimaryActorTick.bCanEverTick = true;
	bReplicates = true;
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

void AAbilityActor::ApplyEffectToTarget(AActor* Target)
{
	if (!HasAuthority())
	{
		UE_LOG(LogTemp, Error, TEXT("No Authority"));
		return;
	}
	if (CasterASC)
	{
		ApplySpecArrayToASC(EffectSpecHandles.SelfSpecs, CasterASC);
	}
	
	UAbilitySystemComponent* TargetASC =
		UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(Target);
	
	if (TargetASC)
	{
		// Apply effects to target
		ApplySpecArrayToASC(EffectSpecHandles.TargetSpecs, TargetASC);
		UE_LOG(LogTemp, Error, TEXT("Target ASC is NULL! %s has no ASC"), *GetNameSafe(Target));
	}
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
	const FAbilityEffectSpecs& InEffectSpecHandles)
{
	if (!InCaster || !InCasterASC)
	{
		UE_LOG(LogTemp, Warning, TEXT("InitializeAbilityActor failed: invalid input"));
		return false;
	}

	Caster = InCaster;
	CasterASC = InCasterASC;
	//should check if struct is valid
	EffectSpecHandles = InEffectSpecHandles;
    
	return true;
}

void AAbilityActor::ApplySpecArrayToASC(const TArray<FGameplayEffectSpecHandle>& Specs, UAbilitySystemComponent* ASC)
{
	if (!ASC)
		return;

	for (const FGameplayEffectSpecHandle& Spec : Specs)
	{
		if (Spec.IsValid())
		{
			ASC->ApplyGameplayEffectSpecToSelf(*Spec.Data.Get());
		}
	}
}

