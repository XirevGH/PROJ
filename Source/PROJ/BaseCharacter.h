#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "InputAction.h"
#include "AbilitySystemInterface.h"
#include "InputActionValue.h"
#include "BaseCharacter.generated.h"

class USpringArmComponent;
class UCameraComponent;	
class USkeletalMeshComponent;
class UCharacterMovementComponent;
class UInputMappingContext;

UCLASS()
class PROJ_API ABaseCharacter : public ACharacter, public IAbilitySystemInterface
{
	GENERATED_BODY()

	
public:
	ABaseCharacter();

	//Ability System
	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;
	
	//AbilitySystem Component
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "AbilitySystem")
	UAbilitySystemComponent* AbilitySystemComponent;
	
	virtual void Tick(float DeltaTime) override;

	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	/* Movement*/
	void InputMove(const FInputActionValue& Value);
	void InputLook(const FInputActionValue& Value);
	void Jump() override;
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
	
protected:
	virtual void BeginPlay() override;

	virtual void PossessedBy(AController* NewController) override;

	virtual void OnRep_PlayerState() override;
};
