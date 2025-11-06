
#include "BaseCharacter.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "AbilitySystemComponent.h"
#include "Engine/LocalPlayer.h"
#include "GameplayAbilitySystem/GP_Dash.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/Controller.h"
#include "GameplayAbilitySystem/BasePlayerState.h"


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

	DefaultAbilities = { UGP_Dash::StaticClass() };
	
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
}

void ABaseCharacter::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);

	ABasePlayerState* PS = GetPlayerState<ABasePlayerState>();
	if (PS && PS->GetAbilitySystemComponent())
	{
		AbilitySystemComponent = PS->GetAbilitySystemComponent();
		AbilitySystemComponent->InitAbilityActorInfo(PS,this);
		//Loopa varje klass default abilities och ge rätt
		PS->GiveDefaultAbilities(DefaultAbilities);
	}
}

void ABaseCharacter::OnRep_PlayerState()
{
	Super::OnRep_PlayerState();
	ABasePlayerState* PS = GetPlayerState<ABasePlayerState>();
	if (PS && PS->GetAbilitySystemComponent())
	{
		AbilitySystemComponent = PS->GetAbilitySystemComponent();
		AbilitySystemComponent->InitAbilityActorInfo(PS,this);
		//Loopa varje klass default abilities och ge rätt
		PS->GiveDefaultAbilities(DefaultAbilities);
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
		EnhancedInput->BindAction(DashAction, ETriggerEvent::Started, this, &ABaseCharacter::ActivateDashAbility);
	}
}

void ABaseCharacter::ActivateDashAbility()
{
	if (AbilitySystemComponent)
	{
		AbilitySystemComponent->TryActivateAbilityByClass(UGP_Dash::StaticClass());
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("AbilitySystemComponent is null!"));
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
