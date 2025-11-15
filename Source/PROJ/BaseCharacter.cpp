
#include "BaseCharacter.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "AbilitySystemComponent.h"
#include "Data/CharacterClassInfo.h"
#include "Engine/LocalPlayer.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/Controller.h"
#include "GameplayAbilitySystem/BaseAbilitySystemComponent.h"
#include "GameplayAbilitySystem/BasePlayerState.h"
#include "GameplayAbilitySystem/AttributeSets/CharacterAttributeSet.h"
#include "Library/BaseAbilitySystemLibrary.h"
#include "./PROJ.h"
#include "./PROJ/GameplayAbilitySystem/GameplayAbilities/BaseGameplayAbility.h"
#include "Weapon/Weapon.h"


ABaseCharacter::ABaseCharacter()
{
	PrimaryActorTick.bCanEverTick = true;
	bReplicates = true;
	GetCharacterMovement()->MaxWalkSpeed = 600.f;
	GetCharacterMovement()->bOrientRotationToMovement = true;
	
}

void ABaseCharacter::BeginPlay()
{
	Super::BeginPlay();

	if (APlayerController* PC = Cast<APlayerController>(GetController()))
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem =
			ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PC->GetLocalPlayer()))
		{
			Subsystem->AddMappingContext(PlayerInputContext, 0);
		}
	}
	
	if (WeaponClass)
	{
		EquippedWeapon = GetWorld()->SpawnActor<AWeapon>(WeaponClass);
		if (EquippedWeapon)
		{
			EquippedWeapon->LocationOffset = FVector(0.f, 0.f, 0.f);
			EquippedWeapon->RotationOffset = FRotator(-90, 0.f, 90.f);
			EquippedWeapon->AttachToCharacter(this, FName("WeaponSocket"));
		}
	}
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
	
	InitAbilitySystemComponent();
	InitAbilityActorInfo();

	//Ska denna vara här?
	InitializeAbilities();
	BindCallbacksToDependencies();
	OnCharacterInitialized();
}

void ABaseCharacter::OnRep_PlayerState()
{
	Super::OnRep_PlayerState();

	if (!BasePlayerState)
	{
		BasePlayerState = GetPlayerState<ABasePlayerState>();
	}
	
	//InitAbilitySystemComponent();
	//InitAbilityActorInfo();
	//InitializeAbilities();
	
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

void ABaseCharacter::OnPrimaryAbility(const FInputActionValue& Value)
{
	SendAbilityLocalInput(Value, static_cast<int32>(EAbilityInputID::Primary));
}

void ABaseCharacter::OnSecondaryAbility(const FInputActionValue& Value)
{
	SendAbilityLocalInput(Value, static_cast<int32>(EAbilityInputID::Secondary));
}

void ABaseCharacter::OnMovementAbility(const FInputActionValue& Value)
{
	SendAbilityLocalInput(Value, static_cast<int32>(EAbilityInputID::Movement));
}

void ABaseCharacter::OnUtilityAbility(const FInputActionValue& Value)
{
	SendAbilityLocalInput(Value, static_cast<int32>(EAbilityInputID::Utility));
}

void ABaseCharacter::SendAbilityLocalInput(const FInputActionValue& Value, int32 InputID) const
{
	if (!BaseAbilitySystemComp.IsValid())
		return;
	
	if (Value.Get<bool>())
	{
		BaseAbilitySystemComp->AbilityLocalInputPressed(InputID);
	}
	else
	{
		BaseAbilitySystemComp->AbilityLocalInputReleased(InputID);
	}
}
/*
void ABaseCharacter::InitializeEffects()
{
	if (!AbilitySystemComponent.IsValid())
		return;
	FGameplayEffectContextHandle EffectContext = AbilitySystemComponent->MakeEffectContext();
	EffectContext.AddSourceObject(this);

	for (TSubclassOf<UGameplayEffect>& Effect : DefaultEffects)
	{
		FGameplayEffectSpecHandle SpecHandle = AbilitySystemComponent->MakeOutgoingSpec(Effect, 1, EffectContext);
		if (SpecHandle.IsValid())
		{
			FActiveGameplayEffectHandle GEHandle = AbilitySystemComponent->ApplyGameplayEffectSpecToSelf(*SpecHandle.Data.Get());
		}
	}
}
*/
void ABaseCharacter::InitializeAbilities()
{
	if (!HasAuthority() || !BaseAbilitySystemComp.IsValid())
		return;
	for (TSubclassOf<UBaseGameplayAbility>& Ability : DefaultAbilities)
	{
		FGameplayAbilitySpecHandle SpecHandle = BaseAbilitySystemComp->GiveAbility(FGameplayAbilitySpec(Ability, 1, static_cast<int32>(Ability.GetDefaultObject()->AbilityInputID), this));
	}
}

void ABaseCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void ABaseCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
	if (UEnhancedInputComponent* EnhancedInput = CastChecked<UEnhancedInputComponent>(PlayerInputComponent))
	{
		EnhancedInput->BindAction(JumpAction, ETriggerEvent::Started, this, &ABaseCharacter::Jump);
		EnhancedInput->BindAction(JumpAction, ETriggerEvent::Completed, this, &ABaseCharacter::StopJumping);
		EnhancedInput->BindAction(MoveAction, ETriggerEvent::Triggered, this, &ABaseCharacter::InputMove);
		EnhancedInput->BindAction(LookAction, ETriggerEvent::Triggered, this, &ABaseCharacter::InputLook);

		EnhancedInput->BindAction(PrimaryAbilityAction, ETriggerEvent::Triggered, this, &ABaseCharacter::OnPrimaryAbility);
		EnhancedInput->BindAction(SecondaryAbilityAction, ETriggerEvent::Triggered, this, &ABaseCharacter::OnSecondaryAbility);
		EnhancedInput->BindAction(MovementAbilityAction, ETriggerEvent::Triggered, this, &ABaseCharacter::OnMovementAbility);
		EnhancedInput->BindAction(UtilityAbilityAction, ETriggerEvent::Triggered, this, &ABaseCharacter::OnUtilityAbility);

		
		if (BaseAbilitySystemComp.IsValid())
		{
			//EnhancedInput->BindAction(ConfirmAbilityAction, ETriggerEvent::Triggered, this &ABaseCharacter::LocalInputConfirm);
			EnhancedInput->BindAction(ConfirmAbilityAction, ETriggerEvent::Triggered, BaseAbilitySystemComp.Get(), &UBaseAbilitySystemComponent::LocalInputConfirm);
			EnhancedInput->BindAction(CancelAbilityAction, ETriggerEvent::Triggered, BaseAbilitySystemComp.Get(), &UBaseAbilitySystemComponent::LocalInputCancel);
		}
	}
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
	FVector2D MoveAxis = Value.Get<FVector2D>();
	if (Controller)
	{
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);

		// Forward/Backward
		const FVector ForwardDir = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
		AddMovementInput(ForwardDir, MoveAxis.Y);

		// Right/Left
		const FVector RightDir = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);
		AddMovementInput(RightDir, MoveAxis.X);
	}
}
void ABaseCharacter::InputLook(const FInputActionValue& Value)
{
	const FVector2D LookAxisValue = Value.Get<FVector2D>();
	if (!GetController())
	{
		UE_LOG(LogTemp, Warning, TEXT("No Controller!"));
		return;
	}
	AddControllerYawInput(LookAxisValue.X * 1.f);
	AddControllerPitchInput(LookAxisValue.Y * 1.f);
}

void ABaseCharacter::Jump()
{
	Super::Jump();
}


