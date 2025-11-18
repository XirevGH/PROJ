
#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemComponent.h"
#include "BaseAbilitySystemComponent.generated.h"

/**
 * 
 */
UCLASS()
class PROJ_API UBaseAbilitySystemComponent : public UAbilitySystemComponent
{
	GENERATED_BODY()
	
public:
	void AddCharacterAbilities(const TArray<TSubclassOf<UGameplayAbility>>& AbilitiesToAdd);
	void AddCharacterPassives(const TArray<TSubclassOf<UGameplayAbility>>& PassivesToAdd);
	void InitializeDefaultAttributes(const TSubclassOf<UGameplayEffect>& AttributeEffect);

	UFUNCTION(BlueprintCallable, Category = "Abilities")
	FGameplayAbilitySpecHandle GetAbilitySpecHandleByTag(FGameplayTag InputTag);
	
	UFUNCTION(BlueprintCallable, Category = "Abilities")
	FGameplayAbilitySpec GetAbilitySpecByHandle(FGameplayAbilitySpecHandle Handle) const;
	
	UFUNCTION(BlueprintCallable, Category = "Abilities")
	TArray<FGameplayAbilitySpec> GetActivatableAbilitySpecs();
	
	void AbilityInputPressed(const FGameplayTag& InputTag);
	void AbilityInputReleased(const FGameplayTag& InputTag);
};
