
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

void ABasePlayerState::InitializeASC(class AActor* Avatar)
{
	BaseAbilitySystemComponent->InitAbilityActorInfo(this, Avatar);
}
