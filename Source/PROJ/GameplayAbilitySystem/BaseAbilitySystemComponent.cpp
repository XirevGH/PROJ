
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
			FGameplayAbilitySpecHandle Handle = GiveAbility(AbilitySpec);

			GrantedAbilityHandles.Add(Handle);
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

FGameplayAbilitySpecHandle UBaseAbilitySystemComponent::GetAbilitySpecHandleByTag(FGameplayTag InputTag) 
{
	if (!InputTag.IsValid()) return FGameplayAbilitySpecHandle();

	ABILITYLIST_SCOPE_LOCK();

	for (const FGameplayAbilitySpec& Spec : GetActivatableAbilities())
	{
		if (Spec.GetDynamicSpecSourceTags().HasTagExact(InputTag))
		{
			return Spec.Handle;
		}
	}
	return FGameplayAbilitySpecHandle();
}

FGameplayAbilitySpec UBaseAbilitySystemComponent::GetAbilitySpecByHandle(FGameplayAbilitySpecHandle Handle) const
{
	return *FindAbilitySpecFromHandle(Handle);
}

TArray<FGameplayAbilitySpec> UBaseAbilitySystemComponent::GetActivatableAbilitySpecs()
{
	TArray<FGameplayAbilitySpec> Specs;

	ABILITYLIST_SCOPE_LOCK();

	for (const FGameplayAbilitySpec& Spec : GetActivatableAbilities())
	{
		Specs.Add(Spec);
	}
	return Specs;
}

UBaseGameplayAbility* UBaseAbilitySystemComponent::GetGameplayAbilityByInputTag(FGameplayTag InputTag)
{
	FGameplayAbilitySpecHandle Handle = GetAbilitySpecHandleByTag(InputTag);
	if (!Handle.IsValid()) return nullptr;

	const FGameplayAbilitySpec* Spec = FindAbilitySpecFromHandle(Handle);
	if (!Spec) return nullptr;

	return Cast<UBaseGameplayAbility>(Spec->Ability);
}

FGameplayAbilitySpecHandle UBaseAbilitySystemComponent::GetSpecHandleFromInputTag(FGameplayTag InputTag)
{
	return GetAbilitySpecHandleByTag(InputTag);
}

TArray<FGameplayAbilitySpecHandle> UBaseAbilitySystemComponent::GetAllAbilitySpecHandles() const
{
	TArray<FGameplayAbilitySpecHandle> SpecHandles;
	for (const auto& Spec : GetActivatableAbilities())
	{
		SpecHandles.Add(Spec.Handle);
	}
	return SpecHandles;
}

FGameplayTag UBaseAbilitySystemComponent::GetInputTagFromHandle(FGameplayAbilitySpecHandle Handle) const
{
	const FGameplayAbilitySpec* Spec = FindAbilitySpecFromHandle(Handle);
	if (!Spec) return FGameplayTag();

	return Spec->GetDynamicSpecSourceTags().First();
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
