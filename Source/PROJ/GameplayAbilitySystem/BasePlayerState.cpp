
#include "BasePlayerState.h"
#include "AbilitySystemComponent.h"
#include "BaseAbilitySystemComponent.h"
#include "AttributeSets/CharacterAttributeSet.h"

ABasePlayerState::ABasePlayerState()
{
	SetNetUpdateFrequency(100.f);
	SetMinNetUpdateFrequency(66.f);
	
	BaseAbilitySystemComponent = CreateDefaultSubobject<UBaseAbilitySystemComponent>(TEXT("AbilitySystem"));
	BaseAbilitySystemComponent->SetIsReplicated(true);
	BaseAbilitySystemComponent->SetReplicationMode(EGameplayEffectReplicationMode::Mixed);
	
	BaseAttributeSet = CreateDefaultSubobject<UCharacterAttributeSet>(TEXT("AttributeSet"));

	bReplicates = true;
}

void ABasePlayerState::BeginPlay()
{
	Super::BeginPlay();
}

UAbilitySystemComponent* ABasePlayerState::GetAbilitySystemComponent() const
{
	return BaseAbilitySystemComponent;
}

UBaseAbilitySystemComponent* ABasePlayerState::GetBaseAbilitySystemComponent() const
{
	return BaseAbilitySystemComponent;
}

UCharacterAttributeSet* ABasePlayerState::GetCharacterAttributeSet() const
{
	return BaseAttributeSet;
}

void ABasePlayerState::GiveDefaultAbilities(const TArray<TSubclassOf<UGameplayAbility>>& Abilities)
{
	if (HasAuthority() && BaseAbilitySystemComponent)
	{
		for (auto& AbilityClass : Abilities)
		{
			UE_LOG(LogTemp, Warning, TEXT("Server: Giving %s to %s"), 
					   *AbilityClass->GetName(), *GetName());
			BaseAbilitySystemComponent->GiveAbility(FGameplayAbilitySpec(AbilityClass, 1, 0, this));
		}
	}
}

void ABasePlayerState::InitializeASC(class AActor* Avatar)
{
	BaseAbilitySystemComponent->InitAbilityActorInfo(this, Avatar);
}
