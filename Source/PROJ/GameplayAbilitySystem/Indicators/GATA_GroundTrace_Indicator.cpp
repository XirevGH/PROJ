// Fill out your copyright notice in the Description page of Project Settings.


#include "GATA_GroundTrace_Indicator.h"
#include "Components/DecalComponent.h"
#include "Components/SceneComponent.h"
#include "Abilities/GameplayAbility.h"
#include "GameFramework/Actor.h"

void AGATA_GroundTrace_Indicator::BeginPlay()
{
	Super::BeginPlay();
	Decal->DecalSize = Size;
}

AGATA_GroundTrace_Indicator::AGATA_GroundTrace_Indicator()
{
	Decal = CreateDefaultSubobject<UDecalComponent>(TEXT("Decal"));
	Root = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	SetRootComponent(Root);
	Decal->SetupAttachment(Root);
	
}

FHitResult AGATA_GroundTrace_Indicator::PerformTrace(AActor* InSourceActor)
{
	FHitResult Hit;
	if (!InSourceActor) return Hit;

	APawn* Pawn = Cast<APawn>(InSourceActor);
	APlayerController* CasterController = Pawn ? Cast<APlayerController>(Pawn->GetController()) : nullptr;
	if (!CasterController)
	{
		UE_LOG(LogTemp, Error, TEXT("No APlayerController"));
		return Hit;
	}

	float MouseX, MouseY;
	if (!CasterController->GetMousePosition(MouseX, MouseY))
	{
		UE_LOG(LogTemp, Warning, TEXT("Can't get mouse position"));
		return Hit;
	}

	FVector WorldOrigin, WorldDirection;
	if (!CasterController->DeprojectScreenPositionToWorld(MouseX, MouseY, WorldOrigin, WorldDirection))
	{
		UE_LOG(LogTemp, Warning, TEXT("Can't deproject screen position"));
		return Hit;
	}
	//DrawDebugSphere(GetWorld(),WorldOrigin, 50.0f, 24,  FColor::Red, true);
	
	// Trace from camera to get mouse world location on the ground
	FVector CameraTraceEnd = WorldOrigin + WorldDirection * 100000.f;
	FCollisionQueryParams TraceParams;
	TraceParams.AddIgnoredActor(this);

	FHitResult MouseHit;
	GetWorld()->LineTraceSingleByChannel(MouseHit, WorldOrigin, CameraTraceEnd, ECC_Visibility, TraceParams);

	// Use either the hit location or the far point
	FVector DesiredLocation = MouseHit.bBlockingHit ? MouseHit.Location : CameraTraceEnd;

	// Clamp to MaxRange from player/caster
	FVector PlayerPosition = InSourceActor->GetActorLocation();
	FVector Direction = DesiredLocation - PlayerPosition;
	float Distance = Direction.Size();

	if (Distance > MaxRange)
	{
		Direction = Direction.GetSafeNormal();
		DesiredLocation = PlayerPosition + Direction * MaxRange;
	}

	// Trace from player to the clamped location to get final hit on ground
	GetWorld()->LineTraceSingleByChannel(Hit, PlayerPosition, DesiredLocation, ECC_Visibility, TraceParams);

	return Hit;
}