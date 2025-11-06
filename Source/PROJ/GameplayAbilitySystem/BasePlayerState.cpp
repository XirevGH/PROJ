
#include "BasePlayerState.h"
#include "AbilitySystemComponent.h"
#include "AttributeSets/CharacterAttributeSet.h"

ABasePlayerState::ABasePlayerState()
{
	AbilitySystemComponent = CreateDefaultSubobject<UAbilitySystemComponent>(TEXT("AbilitySystem"));
	AttributeSet = CreateDefaultSubobject<UCharacterAttributeSet>(TEXT("AttributeSet"));
	
	AbilitySystemComponent->SetIsReplicated(true);
	AbilitySystemComponent->SetReplicationMode(EGameplayEffectReplicationMode::Mixed);
	
	
	
	bReplicates = true;
}

void ABasePlayerState::BeginPlay()
{
	Super::BeginPlay();
}

UAbilitySystemComponent* ABasePlayerState::GetAbilitySystemComponent() const
{
	return AbilitySystemComponent;
}

void ABasePlayerState::GiveDefaultAbilities(const TArray<TSubclassOf<UGameplayAbility>>& Abilities)
{
	if (HasAuthority() && AbilitySystemComponent)
	{
		for (auto& AbilityClass : Abilities)
		{
			UE_LOG(LogTemp, Warning, TEXT("Server: Giving %s to %s"), 
					   *AbilityClass->GetName(), *GetName());
			AbilitySystemComponent->GiveAbility(FGameplayAbilitySpec(AbilityClass, 1, 0));
		}
	}
}