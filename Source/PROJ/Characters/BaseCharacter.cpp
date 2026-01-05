#include "BaseCharacter.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "AbilitySystemComponent.h"
#include "../Data/CharacterClassInfo.h"
#include "Engine/LocalPlayer.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/Controller.h"
#include "../GameplayAbilitySystem/BaseAbilitySystemComponent.h"
#include "../Framework/BasePlayerState.h"
#include "../GameplayAbilitySystem/AttributeSets/CharacterAttributeSet.h"
#include "../Library/BaseAbilitySystemLibrary.h"
#include "../Core/PROJ.h"
#include "./PROJ/GameplayAbilitySystem/GameplayAbilities/BaseGameplayAbility.h"
#include "../Weapon/Weapon.h"

ABaseCharacter::ABaseCharacter()
{
	PrimaryActorTick.bCanEverTick = true;
	bReplicates = true;
	//GetCharacterMovement()->MaxWalkSpeed = 600.f;
	GetCharacterMovement()->bOrientRotationToMovement = false;

	
	bIsFreeLooking = false;
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false; // THIS IS THE MASTER SWITCH
	bUseControllerRotationRoll = false;
	LockedMovementDirection = FRotator::ZeroRotator;
}

void ABaseCharacter::SpawnDefaultWeapon()
{
	if (WeaponClass)
	{
		FActorSpawnParameters Params;
		Params.Owner = this;
		Params.Instigator = this;
		Params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
		
		EquippedWeapon = GetWorld()->SpawnActor<AWeapon>(WeaponClass,Params);
		if (EquippedWeapon)
		{
			EquippedWeapon->LocationOffset = FVector(-9.f, 1.f, 8.f);
			EquippedWeapon->RotationOffset = FRotator(180.f, -90.f, 90.f);
			EquippedWeapon->AttachWeapon();			
			this->EquippedWeapon = EquippedWeapon;
		}
	}
}

void ABaseCharacter::BeginPlay()
{
	Super::BeginPlay();

	BPCameraBoom = FindComponentByClass<USpringArmComponent>();

}

UAbilitySystemComponent* ABaseCharacter::GetAbilitySystemComponent() const
{
	return BaseAbilitySystemComp.Get();
}

void ABaseCharacter::InitAbilitySystemComponent()
{
	if (!BasePlayerState)
		return;
	BaseAbilitySystemComp =  Cast<UBaseAbilitySystemComponent>(BasePlayerState->GetAbilitySystemComponent());
	if (!BaseAbilitySystemComp.IsValid())
		return;
	BaseAbilitySystemComp->InitAbilityActorInfo(BasePlayerState, this);
}

void ABaseCharacter::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);
	
	if (!BasePlayerState)
	{
		BasePlayerState = GetPlayerState<ABasePlayerState>();
	}

	if (APlayerController* PC = Cast<APlayerController>(GetController()))
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem =
			ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PC->GetLocalPlayer()))
		{
			Subsystem->AddMappingContext(PlayerInputContext, 0);
		}
	}
	
	InitAbilitySystemComponent();
	InitAbilityActorInfo();
	
	OnCharacterInitialized();
	SpawnDefaultWeapon();
}

void ABaseCharacter::OnRep_PlayerState()
{
	Super::OnRep_PlayerState();

	if (!BasePlayerState)
	{
		BasePlayerState = GetPlayerState<ABasePlayerState>();
	}

	if (APlayerController* PC = Cast<APlayerController>(GetController()))
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem =
			ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PC->GetLocalPlayer()))
		{
			Subsystem->AddMappingContext(PlayerInputContext, 0);
		}
	}
	
	InitAbilitySystemComponent();
	InitAbilityActorInfo();
	
	OnCharacterInitialized();
}

void ABaseCharacter::OnHealthAttributeChanged(const FOnAttributeChangeData& Data)
{
	OnHealthChanged(Data.NewValue, BaseAttributes->GetMaxHealth());
}

void ABaseCharacter::OnMoveSpeedAttributeChanged(const FOnAttributeChangeData& Data)
{
	UE_LOG(LogTemp, Warning, TEXT("Movespeed set to %f"), Data.NewValue);
	OnMoveSpeedChanged(Data.NewValue, BaseAttributes->GetMaxMoveSpeed());
}

void ABaseCharacter::OnManaAttributeChanged(const FOnAttributeChangeData& Data)
{
	OnManaChanged(Data.NewValue, BaseAttributes->GetMaxMana());
}

void ABaseCharacter::Server_SetFreeLooking_Implementation(bool bNewFreeLooking)
{
	bIsFreeLooking = bNewFreeLooking;
	if (bIsFreeLooking)
	{
		GetCharacterMovement()->bOrientRotationToMovement = false;
	}
}

void ABaseCharacter::Server_SetUseControllerRotationYaw_Implementation(bool bNewUseControllerRotationYaw)
{
	bUseControllerRotationYaw = bNewUseControllerRotationYaw;
}

void ABaseCharacter::HandleDamageDealt(AActor* TargetActor, float DamageAmount, bool bIsCrit)
{
	// We call this on the ATTACKER. 
	// This sends the message to the Player Controller of the person who shot the gun.
	UE_LOG(LogTemp, Warning, TEXT("ClientShowDamageNumber"));

	ClientShowDamageNumber(TargetActor, DamageAmount, bIsCrit);
}

void ABaseCharacter::ClientShowDamageNumber_Implementation(AActor* TargetActor, float DamageAmount, bool bIsCrit)
{
	// Now we are on the Player's screen.
	// We have the reference to the TargetActor (the enemy) here!
	UE_LOG(LogTemp, Warning, TEXT("OnShowDamageNumber"));
	OnShowDamageNumber(TargetActor, DamageAmount, bIsCrit);
}

void ABaseCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (BPCameraBoom)
	{
		// Get the current length
		float CurrentLength = BPCameraBoom->TargetArmLength;

		// Check if we are close enough; if not, interpolate
		if (!FMath::IsNearlyEqual(CurrentLength, DesiredArmLength, 0.1f))
		{
			// FInterpTo creates a smooth "ease-out" movement
			float NewLength = FMath::FInterpTo(
				CurrentLength,      // Where we are
				DesiredArmLength,   // Where we want to be
				DeltaTime,          // Time since last frame
				ZoomInterpSpeed     // How fast to go
			);

			// Apply the smoothed value
			BPCameraBoom->TargetArmLength = NewLength;
		}
	}
}

void ABaseCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
	if (UEnhancedInputComponent* EnhancedInput = CastChecked<UEnhancedInputComponent>(PlayerInputComponent))
	{
		EnhancedInput->BindAction(JumpAction, ETriggerEvent::Started, this, &ABaseCharacter::Jump);
		EnhancedInput->BindAction(JumpAction, ETriggerEvent::Completed, this, &ABaseCharacter::StopJumping);
		EnhancedInput->BindAction(MoveAction, ETriggerEvent::Triggered, this, &ABaseCharacter::InputMove);
		
		EnhancedInput->BindAction(MouseMoveAction, ETriggerEvent::Triggered, this, &ABaseCharacter::InputMouseMoveTriggered);
		EnhancedInput->BindAction(MouseMoveAction, ETriggerEvent::Completed, this, &ABaseCharacter::InputMouseMoveCompleted);
		EnhancedInput->BindAction(LookAction, ETriggerEvent::Triggered, this, &ABaseCharacter::InputLook);
		
		EnhancedInput->BindAction(RotateCameraAction, ETriggerEvent::Started, this, &ABaseCharacter::InputRotateCameraStarted);
		EnhancedInput->BindAction(RotateCameraAction, ETriggerEvent::Completed, this, &ABaseCharacter::InputRotateCameraCompleted);
		
		EnhancedInput->BindAction(RotateCharacterAction, ETriggerEvent::Started, this, &ABaseCharacter::InputRotateCharacterStarted);
		EnhancedInput->BindAction(RotateCharacterAction, ETriggerEvent::Triggered, this, &ABaseCharacter::InputRotateCharacterTriggered);
		EnhancedInput->BindAction(RotateCharacterAction, ETriggerEvent::Completed, this, &ABaseCharacter::InputRotateCharacterCompleted);

		EnhancedInput->BindAction(ZoomAction, ETriggerEvent::Triggered, this, &ABaseCharacter::InputZoom);
		
		APlayerController* PC = Cast<APlayerController>(GetController());
		if (!PC) return;

		ABasePlayerState* PS = PC->GetPlayerState<ABasePlayerState>();
		if (!PS) return;

		UAbilitySystemComponent* ASC = PS->GetAbilitySystemComponent();
		if (!ASC) return;
		
		if (ASC)
		{
			EnhancedInput->BindAction(CancelActiveAbilityAction, ETriggerEvent::Triggered, this, &ABaseCharacter::OnCancelActiveAbilityTriggered);
			
			EnhancedInput->BindAction(ConfirmAbilityAction, ETriggerEvent::Triggered, ASC, &UBaseAbilitySystemComponent::TargetConfirm);
		 	EnhancedInput->BindAction(CancelAbilityAction, ETriggerEvent::Triggered, ASC, &UBaseAbilitySystemComponent::TargetCancel);
		}
	}
}

void ABaseCharacter::InputZoom(const FInputActionValue& Value)
{
	float AxisValue = Value.Get<float>();

	if (AxisValue != 0.0f)
	{
		DesiredArmLength -= (AxisValue * ZoomStep);
		DesiredArmLength = FMath::Clamp(DesiredArmLength, MinZoomDistance, MaxZoomDistance);
	}
}

void ABaseCharacter::InputMouseMoveTriggered(const FInputActionValue& Value)
{
	UE_LOG(LogTemp, Display, TEXT("InputMouseMoveTriggered"));
	bUseControllerRotationYaw = true;
	Server_SetUseControllerRotationYaw(true);
	
	FVector2D MoveAxis = Value.Get<FVector2D>();
	if ((Controller != nullptr) && (MoveAxis.X != 0.0f || MoveAxis.Y != 0.0f))
	{
		const FRotator BaseRotation = GetController()->GetControlRotation();

		const FRotator YawRotation(0, BaseRotation.Yaw, 0);

		// Get world-space forward and right vectors from our chosen base rotation
		const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
		
		// Add the movement input
		AddMovementInput(ForwardDirection, MoveAxis.X);
	}
}

void ABaseCharacter::InputMouseMoveCompleted(const FInputActionValue& Value)
{
	UE_LOG(LogTemp, Display, TEXT("InputMouseMoveCompleted"));
	bUseControllerRotationYaw = false;
	Server_SetUseControllerRotationYaw(false);
}


void ABaseCharacter::InitAbilityActorInfo()
{
	if (IsValid(BasePlayerState))
	{
		BaseAbilitySystemComp = BasePlayerState->GetBaseAbilitySystemComponent();
		BaseAttributes = BasePlayerState->GetCharacterAttributeSet();

		if (BaseAbilitySystemComp.IsValid())
		{
			BaseAbilitySystemComp->InitAbilityActorInfo(BasePlayerState, this);

			BindCallbacksToDependencies();
			if (HasAuthority())
			{
				InitClassDefaults();
			}
		}
	}
}

void ABaseCharacter::InitClassDefaults() const
{
	if (!CharacterTag.IsValid())
	{
		UE_LOG(LogTemp, Warning, TEXT("No CharacterTag is selected in this char %s"), *GetNameSafe(this));
	}
	else if (UCharacterClassInfo* ClassInfo = UBaseAbilitySystemLibrary::GetCharacterClassDefaultInfo(this))
	{
		if (const FCharacterClassDefaultInfo* SelectedClassInfo = ClassInfo->ClassDefaultInfoMap.Find(CharacterTag))
		{
			if (BaseAbilitySystemComp.IsValid())
			{
				BaseAbilitySystemComp->AddCharacterAbilities(SelectedClassInfo->StartingAbilities);
				BaseAbilitySystemComp->AddCharacterPassives(SelectedClassInfo->StartingPassives);
				BaseAbilitySystemComp->InitializeDefaultAttributes(SelectedClassInfo->DefaultAttributes);
			}
		}
	}
}

void ABaseCharacter::BindCallbacksToDependencies()
{
	if (BaseAbilitySystemComp.IsValid() && IsValid(BaseAttributes))
	{
		BaseAbilitySystemComp->GetGameplayAttributeValueChangeDelegate(
			BaseAttributes->GetCurrentHealthAttribute()).
			AddUObject(this, &ABaseCharacter::OnHealthAttributeChanged);
		
		BaseAbilitySystemComp->GetGameplayAttributeValueChangeDelegate(
			BaseAttributes->GetManaAttribute()).
			AddUObject(this, &ABaseCharacter::OnManaAttributeChanged);

		BaseAbilitySystemComp->GetGameplayAttributeValueChangeDelegate(
		BaseAttributes->GetCurrentMoveSpeedAttribute()).
		AddUObject(this, &ABaseCharacter::OnMoveSpeedAttributeChanged);
	}
}

void ABaseCharacter::BroadcastInitialValues()
{
	if (IsValid(BaseAttributes))
	{
		OnHealthChanged(BaseAttributes->GetCurrentHealth(), BaseAttributes->GetMaxHealth());
		OnManaChanged(BaseAttributes->GetMana(), BaseAttributes->GetMaxMana());
		OnMoveSpeedChanged(BaseAttributes->GetCurrentMoveSpeed(), BaseAttributes->GetMaxMoveSpeed());
	}
}

void ABaseCharacter::InputMove(const FInputActionValue& Value)
{
	if (bMovementInputBlocked) return;
	FVector2D MoveAxis = Value.Get<FVector2D>();
	if ((Controller != nullptr) && (MoveAxis.X != 0.0f || MoveAxis.Y != 0.0f))
	{
		const FRotator BaseRotation = bUseControllerRotationYaw ? GetController()->GetControlRotation() : GetActorRotation();

		const FRotator YawRotation(0, BaseRotation.Yaw, 0);

		// Get world-space forward and right vectors from our chosen base rotation
		const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
		const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);

		// Add the movement input
		AddMovementInput(ForwardDirection, MoveAxis.Y);
		AddMovementInput(RightDirection, MoveAxis.X);
	}
}
void ABaseCharacter::InputLook(const FInputActionValue& Value)
{
	UE_LOG(LogTemp, Display, TEXT("InputLook"));
	const FVector2D LookVector = Value.Get<FVector2D>();
	if (Controller != nullptr)
	{
		AddControllerYawInput(LookVector.X);
		AddControllerPitchInput(LookVector.Y);
	}
}

void ABaseCharacter::InputRotateCameraStarted(const FInputActionValue& Value)
{
	UE_LOG(LogTemp, Display, TEXT("InputRotateCameraStarted"));
	bIsFreeLooking = true;
	LockedMovementRotation = GetActorRotation();
	GetCharacterMovement()->bOrientRotationToMovement = false;
	Server_SetFreeLooking(true);
}

void ABaseCharacter::InputRotateCameraCompleted(const FInputActionValue& Value)
{
	UE_LOG(LogTemp, Display, TEXT("InputRotateCameraCompleted"));
	bIsFreeLooking = false;
	Server_SetFreeLooking(false);
}

void ABaseCharacter::InputRotateCharacterStarted(const FInputActionValue& Value)
{
	UE_LOG(LogTemp, Display, TEXT("InputRotateCharacterStarted"));

	bUseControllerRotationYaw = true;
	Server_SetUseControllerRotationYaw(true);
}

void ABaseCharacter::InputRotateCharacterCompleted(const FInputActionValue& Value)
{
	UE_LOG(LogTemp, Display, TEXT("InputRotateCharacterCompleted"));

	bUseControllerRotationYaw = false;
	Server_SetUseControllerRotationYaw(false);
}

void ABaseCharacter::InputRotateCharacterTriggered(const FInputActionValue& Value)
{
	UE_LOG(LogTemp, Display, TEXT("InputRotateCharacterTriggered"));
	bUseControllerRotationYaw = true;
	Server_SetUseControllerRotationYaw(true);
	LockedMovementRotation = GetActorRotation();
}

void ABaseCharacter::Jump()
{
	Super::Jump();
}

void ABaseCharacter::Landed(const FHitResult& Hit)
{
	Super::Landed(Hit);
	OnCharacterLanded.Broadcast();
}

void ABaseCharacter::OnCancelActiveAbilityTriggered(const FInputActionValue& Value)
{
	APlayerController* PC = Cast<APlayerController>(GetController());
	if (!PC) return;

	ABasePlayerState* PS = PC->GetPlayerState<ABasePlayerState>();
	if (!PS) return;

	UAbilitySystemComponent* ASC = PS->GetAbilitySystemComponent();
	if (!ASC) return;
	
		FGameplayTagContainer CancelTags;
		CancelTags.AddTag(FGameplayTag::RequestGameplayTag("Ability"));
		UE_LOG(LogTemp, Warning, TEXT("cancel ability"));
		ASC->CancelAbilities(&CancelTags, nullptr);
	
}
