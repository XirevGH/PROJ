#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "GameFramework/Character.h"
#include "InputAction.h"

#include "InputActionValue.h"
#include "BaseCharacter.generated.h"

struct FOnAttributeChangeData;
class UBaseAbilitySystemComponent;
class UCharacterAttributeSet;
class UAbilitySystemComponent;
class USpringArmComponent;
class UCameraComponent;	
class USkeletalMeshComponent;
class UCharacterMovementComponent;
class UInputMappingContext;
class UGameplayAbility;
class UHealthComponent;

UCLASS()
class PROJ_API ABaseCharacter : public ACharacter
{
	GENERATED_BODY()

	
public:
	ABaseCharacter();
	
	virtual void Tick(float DeltaTime) override;

	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	/* Movement*/
	void InputMove(const FInputActionValue& Value);
	void InputLook(const FInputActionValue& Value);
	void Jump() override;

	UFUNCTION(BlueprintImplementableEvent)
	void OnHealthChanged(float CurrentHealth, float MaxHealth);
	UFUNCTION(BlueprintImplementableEvent)
	void OnManaChanged(float CurrentMana, float MaxMana);

	void HandleHealthChanged(const FOnAttributeChangeData& Data);
	void HandleManaChanged(const FOnAttributeChangeData& Data);
	
	UPROPERTY(VisibleAnywhere)
	UCharacterMovementComponent* MovementComponent;
	
	UPROPERTY(EditAnywhere)
	UCameraComponent* Camera;

	UPROPERTY(EditAnywhere)
	USpringArmComponent* SpringArm;

	UPROPERTY(EditAnywhere)
	USkeletalMeshComponent* SkeletalMesh;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, category = "Enhanced Input")
	UInputMappingContext* PlayerInputContext;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, category = "Enhanced Input")
	UInputAction* LookAction;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, category = "Enhanced Input")
	UInputAction* JumpAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, category = "Enhanced Input")
	UInputAction* MoveAction;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, category = "Enhanced Input")
	UInputAction* DashAction;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, category = "Gameplay ability system")
	UAbilitySystemComponent* AbilitySystemComponent;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Abilities")
	TArray<TSubclassOf<UGameplayAbility>> DefaultAbilities;
	/* Exemple	*/
	/************/
	/* WarriorCharacter.cpp
	AWarriorCharacter::AWarriorCharacter()
	{
		DefaultAbilities = { UGP_Slash::StaticClass(), UGP_ShieldBash::StaticClass() };
	}*/

protected:
	virtual void BeginPlay() override;

	virtual void PossessedBy(AController* NewController) override;

	virtual void OnRep_PlayerState() override;

	UFUNCTION(BlueprintCallable)
	void ActivateDashAbility();
private:
	UPROPERTY(BlueprintReadOnly, meta = (AllowPrivateAccess = true))
	TObjectPtr<UBaseAbilitySystemComponent> BaseAbilitySystemComp;

	UPROPERTY(BlueprintReadOnly, meta = (AllowPrivateAccess = true))
	TObjectPtr<UCharacterAttributeSet> BaseAttributes;

	UPROPERTY(EditAnywhere, Category = "Custom Values|Character Info")
	FGameplayTag CharacterTag;
	
	void InitAbilityActorInfo();
	void InitClassDefaults();
	void BindCallbacksToDependencies();

	UFUNCTION(BlueprintCallable)
	void BroadcastIntialValues();
};
