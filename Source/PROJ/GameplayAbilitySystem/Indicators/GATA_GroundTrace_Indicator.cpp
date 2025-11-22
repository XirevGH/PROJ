// Fill out your copyright notice in the Description page of Project Settings.


#include "GATA_GroundTrace_Indicator.h"

#include "CollisionDebugDrawingPublic.h"
#include "Components/DecalComponent.h"
#include "Components/SceneComponent.h"
#include "GameFramework/Actor.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "PROJ/Characters/BaseCharacter.h"


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
	
	//DrawDebugLine(GetWorld(),WorldOrigin, CameraTraceEnd, FColor::Red, true);
	
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
	TraceParams.bTraceComplex = true;
    TraceParams.AddIgnoredActor(MouseHit.GetActor());
	
	FVector GroundTraceStart = DesiredLocation;
	FVector GroundTraceEnd = DesiredLocation - FVector(0, 0, MaxRange + MaxRange);   // trace down
	GetWorld()->LineTraceSingleByChannel(Hit, GroundTraceStart, GroundTraceEnd, ECC_Visibility, TraceParams);
	if (!Hit.GetActor())
	{
		TraceParams.ClearIgnoredSourceObjects();
		TraceParams.AddIgnoredActor(this);
		GetWorld()->LineTraceSingleByChannel(Hit, GroundTraceStart, GroundTraceEnd, ECC_Visibility);
	}
	
	DrawDebugLine(GetWorld(),GroundTraceStart, GroundTraceEnd, FColor::Green, true);
	

	return Hit;
}