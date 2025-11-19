
#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemComponent.h"
#include "BaseAbilitySystemComponent.generated.h"

class UBaseGameplayAbility;
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

	UPROPERTY(BlueprintReadOnly)
	TArray<FGameplayAbilitySpecHandle> GrantedAbilityHandles;

	UFUNCTION(BlueprintCallable, Category = "Abilities")
	FGameplayAbilitySpecHandle GetAbilitySpecHandleByTag(FGameplayTag InputTag);
	
	UFUNCTION(BlueprintCallable, Category = "Abilities")
	FGameplayAbilitySpec GetAbilitySpecByHandle(FGameplayAbilitySpecHandle Handle) const;
	
	UFUNCTION(BlueprintCallable, Category = "Abilities")
	TArray<FGameplayAbilitySpec> GetActivatableAbilitySpecs();

	UFUNCTION(BlueprintCallable, Category="Abilities")
	UBaseGameplayAbility* GetGameplayAbilityByInputTag(FGameplayTag InputTag);

	UFUNCTION(BlueprintCallable, Category="Abilities")
	FGameplayAbilitySpecHandle GetSpecHandleFromInputTag(FGameplayTag InputTag);

	UFUNCTION(BlueprintCallable, Category="Abilities")
	TArray<FGameplayAbilitySpecHandle> GetAllAbilitySpecHandles() const;
	
	UFUNCTION(BlueprintCallable, Category="Abilities")
	FGameplayTag GetInputTagFromHandle(FGameplayAbilitySpecHandle Handle) const;
	
	void AbilityInputPressed(const FGameplayTag& InputTag);
	void AbilityInputReleased(const FGameplayTag& InputTag);
};
