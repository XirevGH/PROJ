
#include "BasePlayerState.h"
#include "AbilitySystemComponent.h"
#include "Net/UnrealNetwork.h"
#include "PROJ/GameplayAbilitySystem/BaseAbilitySystemComponent.h"
#include "PROJ/GameplayAbilitySystem/AttributeSets/CharacterAttributeSet.h"

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

void ABasePlayerState::SetTeamID(const FString& NewTeamID)
{
		TeamID = NewTeamID;
		OnRep_TeamID();
		UE_LOG(LogTemp, Display, TEXT("TeamID: %s"), *GetTeamID());
}

void ABasePlayerState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	// This command is what actually makes the engine send the data
	DOREPLIFETIME(ABasePlayerState, TeamID);
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
