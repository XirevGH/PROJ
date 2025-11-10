#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "GameFramework/Character.h"
#include "InputAction.h"
#include "AbilitySystemInterface.h"
#include "InputActionValue.h"
#include "GameplayAbilitySystem/BasePlayerState.h"
#include "GameplayAbilitySystem/AttributeSets/CharacterAttributeSet.h"
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
class PROJ_API ABaseCharacter : public ACharacter, public IAbilitySystemInterface
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

	UFUNCTION(BlueprintImplementableEvent, Category = "GAS")
	void OnHealthChanged(float CurrentHealth, float MaxHealth);
	UFUNCTION(BlueprintImplementableEvent, Category = "GAS")
	void OnManaChanged(float CurrentMana, float MaxMana);
	
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

	UPROPERTY(EditAnywhere, BlueprintReadOnly, category = "Enhanced Input")
	UInputAction* PrimaryAbilityAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, category = "Enhanced Input")
	UInputAction* SecondaryAbilityAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, category = "Enhanced Input")
	UInputAction* MovementAbilityAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, category = "Enhanced Input")
	UInputAction* UtilityAbilityAction;

	UPROPERTY(EditAnywhere ,BlueprintReadWrite, Category = "GAS")
	TArray<TSubclassOf<class UGameplayEffect>> DefaultEffects;
	
	UPROPERTY(EditAnywhere,BlueprintReadWrite, Category = "GAS")
	TArray<TSubclassOf<class UBaseGameplayAbility>> DefaultAbilities;

protected:
	virtual void BeginPlay() override;
	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;

	void InitAbilitySystemComponent();

	virtual void PossessedBy(AController* NewController) override;
	virtual void OnRep_PlayerState() override;

	UFUNCTION(BlueprintImplementableEvent, Category = "PlayerState")
	void OnPlayerStateReplicated();

	UPROPERTY(BlueprintReadOnly, Category = "PlayerState")
	ABasePlayerState* BasePlayerState;
	
	virtual void OnHealthAttributeChanged(const FOnAttributeChangeData& Data);
	virtual void OnManaAttributeChanged(const FOnAttributeChangeData& Data);
	
	void OnPrimaryAbility(const FInputActionValue& Value);
	void OnSecondaryAbility(const FInputActionValue& Value);
	void OnMovementAbility(const FInputActionValue& Value);
	void OnUtilityAbility(const FInputActionValue& Value);

	void SendAbilityLocalInput(const FInputActionValue& Value, int32 InputID) const;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, category = "GAS")
	TWeakObjectPtr<class UAbilitySystemComponent> AbilitySystemComponent;
private:
	//void InitializeEffects();
	void InitializeAbilities();

	UPROPERTY(BlueprintReadOnly, meta = (AllowPrivateAccess = true))
	TWeakObjectPtr<UBaseAbilitySystemComponent> BaseAbilitySystemComp;

	UPROPERTY(BlueprintReadOnly, meta = (AllowPrivateAccess = true))
	TObjectPtr<UCharacterAttributeSet> BaseAttributes;

	UPROPERTY(EditAnywhere, Category = "Custom Values|Character Info")
	FGameplayTag CharacterTag;
	
	void InitAbilityActorInfo();
	void InitClassDefaults() const;
	void BindCallbacksToDependencies();

	UFUNCTION(BlueprintCallable)
	void BroadcastInitialValues();
};


