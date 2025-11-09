
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
#include "GameplayAbilitySystem/GameplayAbilities/GA_Basic_Attack.h"
#include "Library/BaseAbilitySystemLibrary.h"
#include "./PROJ.h"
#include "./PROJ/GameplayAbilitySystem/GameplayAbilities/BaseGameplayAbility.h"


ABaseCharacter::ABaseCharacter()
{
	PrimaryActorTick.bCanEverTick = true;

	GetCharacterMovement()->MaxWalkSpeed = 600.f;
	GetCharacterMovement()->bOrientRotationToMovement = true;
	//SpringArm
	SpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArm"));
	SpringArm->SetupAttachment(RootComponent);  
	SpringArm->TargetArmLength = 450.f;
	SpringArm->bUsePawnControlRotation = true;
	//Camera
	Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	Camera->SetupAttachment(SpringArm, USpringArmComponent::SocketName);
	Camera->bUsePawnControlRotation = false;
	
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

	ABasePlayerState* PS = GetPlayerState<ABasePlayerState>();
	if (!PS)
		return;
	UCharacterAttributeSet* AttributeSet = PS->AttributeSet;

	AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(AttributeSet->GetCurrentHealthAttribute()).AddUObject(this, &ABaseCharacter::OnHealthAttributeChanged);
}

UAbilitySystemComponent* ABaseCharacter::GetAbilitySystemComponent() const
{
	return AbilitySystemComponent.Get();
}

void ABaseCharacter::InitAbilitySystemComponent()
{
	ABasePlayerState* PS = GetPlayerState<ABasePlayerState>();
	if (!PS)
		return;
	AbilitySystemComponent = PS->GetAbilitySystemComponent();
	if (!AbilitySystemComponent.IsValid())
		return;
	AbilitySystemComponent->InitAbilityActorInfo(PS, this);
}

void ABaseCharacter::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);
	InitAbilitySystemComponent();
	
	InitializeEffects();
	if (HasAuthority())
	{
		InitAbilityActorInfo();
		ABasePlayerState* PS = GetPlayerState<ABasePlayerState>();
		PS->GiveDefaultAbilities(DefaultAbilities);
	}
	InitAbilitySystemComponent();
	
	InitializeEffects();
}

void ABaseCharacter::OnRep_PlayerState()
{
	Super::OnRep_PlayerState();
	
	InitAbilityActorInfo();
	InitAbilitySystemComponent();

	InitializeEffects();
}

void ABaseCharacter::OnHealthAttributeChanged(const FOnAttributeChangeData& Data)
{
	OnHealthChanged(Data.OldValue, Data.NewValue);
}

void ABaseCharacter::OnPrimaryAbility(const FInputActionValue& Value)
{
	SendAbilityLocalInput(Value, static_cast<int32>(EAbilityInputID::PrimaryAbility));
}

void ABaseCharacter::OnSecondaryAbility(const FInputActionValue& Value)
{
	SendAbilityLocalInput(Value, static_cast<int32>(EAbilityInputID::SecondaryAbility));
}

void ABaseCharacter::OnMovementAbility(const FInputActionValue& Value)
{
	SendAbilityLocalInput(Value, static_cast<int32>(EAbilityInputID::MovementAbility));
}

void ABaseCharacter::OnUtilityAbility(const FInputActionValue& Value)
{
	SendAbilityLocalInput(Value, static_cast<int32>(EAbilityInputID::UtilityAbility));
}

void ABaseCharacter::SendAbilityLocalInput(const FInputActionValue& Value, int32 InputID)
{
	if (!AbilitySystemComponent.IsValid())
		return;
	
	if (Value.Get<bool>())
	{
		AbilitySystemComponent->AbilityLocalInputPressed(InputID);
	}
	else
	{
		AbilitySystemComponent->AbilityLocalInputReleased(InputID);
	}
}

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

void ABaseCharacter::InitializeAbilities()
{
	if (!HasAuthority() || !AbilitySystemComponent.IsValid())
		return;
	for (TSubclassOf<UBaseGameplayAbility>& Ability : DefaultAbilities)
	{
		FGameplayAbilitySpecHandle SpecHandle = AbilitySystemComponent->GiveAbility(FGameplayAbilitySpec(Ability, 1, static_cast<int32>(Ability.GetDefaultObject()->AbilityInputID), this));
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
	}
}


void ABaseCharacter::InitAbilityActorInfo()
{
	if (ABasePlayerState* PS = GetPlayerState<ABasePlayerState>())
	{
		BaseAbilitySystemComp = PS->GetBaseAbilitySystemComponent();
		BaseAttributes = PS->GetCharacterAttributeSet();

		if (IsValid(BaseAbilitySystemComp))
		{
			BaseAbilitySystemComp->InitAbilityActorInfo(PS, this);
			BindCallbacksToDependencies();

			if (HasAuthority())
			{
				InitClassDefaults();
			}
		}
	}
}

void ABaseCharacter::InitClassDefaults()
{
	if (!CharacterTag.IsValid())
	{
		UE_LOG(LogTemp, Warning, TEXT("No CharacterTag is selected in this char %s"), *GetNameSafe(this));
	}
	else if (UCharacterClassInfo* ClassInfo = UBaseAbilitySystemLibrary::GetCharacterClassDefaultInfo(this))
	{
		if (const FCharacterClassDefaultInfo* SelectedClassInfo = ClassInfo->ClassDefaultInfoMap.Find(CharacterTag))
		{
			if (IsValid(BaseAbilitySystemComp))
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
	if (IsValid(BaseAbilitySystemComp) && IsValid(BaseAttributes))
	{
		BaseAbilitySystemComp->GetGameplayAttributeValueChangeDelegate(
			BaseAttributes->GetCurrentHealthAttribute()).
			AddUObject(this, &ABaseCharacter::HandleHealthChanged);
		
		BaseAbilitySystemComp->GetGameplayAttributeValueChangeDelegate(
			BaseAttributes->GetManaAttribute()).
			AddUObject(this, &ABaseCharacter::HandleManaChanged);
	}
}

void ABaseCharacter::BroadcastInitialValues()
{
	if (IsValid(BaseAttributes))
	{
		OnHealthChanged(BaseAttributes->GetCurrentHealth(), BaseAttributes->GetMaxHealth());
		OnManaChanged(BaseAttributes->GetMana(), BaseAttributes->GetMaxMana());
	}
}
void ABaseCharacter::HandleHealthChanged(const FOnAttributeChangeData& Data)
{
	OnHealthChanged(Data.NewValue, BaseAttributes->GetMaxHealth());
}

void ABaseCharacter::HandleManaChanged(const FOnAttributeChangeData& Data)
{
	OnManaChanged(Data.NewValue, BaseAttributes->GetMaxMana());
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


