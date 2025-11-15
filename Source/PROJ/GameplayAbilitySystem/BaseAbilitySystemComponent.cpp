
#include "BaseAbilitySystemComponent.h"

#include "InputTriggers.h"
#include "GameplayAbilities/BaseGameplayAbility.h"

void UBaseAbilitySystemComponent::AddCharacterAbilities(const TArray<TSubclassOf<UGameplayAbility>>& AbilitiesToAdd)
{
	for (const TSubclassOf<UGameplayAbility>& Ability : AbilitiesToAdd)
	{
		FGameplayAbilitySpec AbilitySpec = FGameplayAbilitySpec(Ability,1.f);

		if (const UBaseGameplayAbility* BaseAbility = Cast<UBaseGameplayAbility>(AbilitySpec.Ability))
		{
			AbilitySpec.GetDynamicSpecSourceTags().AddTag(BaseAbility->InputTag);
			GiveAbility(AbilitySpec);
		}
	}
}

void UBaseAbilitySystemComponent::AddCharacterPassives(const TArray<TSubclassOf<UGameplayAbility>>& PassivesToAdd)
{
	for (const TSubclassOf<UGameplayAbility>& Ability : PassivesToAdd)
	{
		FGameplayAbilitySpec AbilitySpec = FGameplayAbilitySpec(Ability,1.f);
		GiveAbilityAndActivateOnce(AbilitySpec);
	}
}

void UBaseAbilitySystemComponent::InitializeDefaultAttributes(const TSubclassOf<UGameplayEffect>& AttributeEffect)
{
	checkf(AttributeEffect, TEXT("No valid attributes for this character %s"), *GetAvatarActor()->GetName())
	
	const FGameplayEffectContextHandle ContextHandle = MakeEffectContext();
	const FGameplayEffectSpecHandle SpecHandle = MakeOutgoingSpec(AttributeEffect, 1.f,ContextHandle);
	ApplyGameplayEffectSpecToSelf(*SpecHandle.Data.Get());
}

void UBaseAbilitySystemComponent::AbilityInputPressed(const FGameplayTag& InputTag)
{
	if (!InputTag.IsValid()) return;
	
	ABILITYLIST_SCOPE_LOCK();

	for (const FGameplayAbilitySpec& Spec: GetActivatableAbilities())
	{
		if (Spec.GetDynamicSpecSourceTags().HasTagExact(InputTag))
		{
			if (!Spec.IsActive())
			{
				TryActivateAbility(Spec.Handle);
			}
			else
			{
				// Get all active instances of this ability
				TArray<UGameplayAbility*> Instances = Spec.GetAbilityInstances();
				for (const UGameplayAbility* Ability : Instances)
				{
					if (Ability)
					{
						InvokeReplicatedEvent(EAbilityGenericReplicatedEvent::InputPressed, Spec.Handle,
						Ability->GetCurrentActivationInfo().GetActivationPredictionKey());
					}
				}
			}
		}
	}
}

void UBaseAbilitySystemComponent::AbilityInputReleased(const FGameplayTag& InputTag)
{
	if (!InputTag.IsValid()) return;

	ABILITYLIST_SCOPE_LOCK();

	for (const FGameplayAbilitySpec& Spec: GetActivatableAbilities())
	{
		if (Spec.GetDynamicSpecSourceTags().HasTagExact(InputTag))
		{
			if (!Spec.IsActive())
			{
				TArray<UGameplayAbility*> Instances = Spec.GetAbilityInstances();
				for (const UGameplayAbility* Ability : Instances)
				{
					if (Ability)
					{
						InvokeReplicatedEvent(EAbilityGenericReplicatedEvent::InputPressed, Spec.Handle,
						Ability->GetCurrentActivationInfo().GetActivationPredictionKey());
					}
				}
			}
		}
	}
}
