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
	
	// First trace: camera → mouse direction
	FVector CameraTraceEnd = WorldOrigin + WorldDirection * 100000.f;
	FCollisionQueryParams Params;
	Params.AddIgnoredActor(this);
	Params.bTraceComplex = true;

	FHitResult MouseHit;
	GetWorld()->LineTraceSingleByChannel(MouseHit, WorldOrigin, CameraTraceEnd, ECC_Visibility, Params);

	// Fallback if nothing hit
	FVector DesiredLocation = MouseHit.bBlockingHit ? MouseHit.Location : CameraTraceEnd;
	
	//Clamp to ability range
	FVector PlayerPos = InSourceActor->GetActorLocation();
	FVector Dir = DesiredLocation - PlayerPos;
	float Dist = Dir.Size();

	if (Dist > MaxRange)
	{
		Dir = Dir.GetSafeNormal();
		DesiredLocation = PlayerPos + Dir * MaxRange;
	}
	FVector DownStart = DesiredLocation + FVector(0,0,10); // lift a little for safety
	FVector DownEnd   = DesiredLocation - FVector(0,0,5000);
	
	// STEP 2 — If surface hit was VALID, return it (roof allowed)
	if (MouseHit.bBlockingHit)
	{
		float Angle = FMath::RadiansToDegrees(
			acosf(FVector::DotProduct(MouseHit.ImpactNormal, FVector::UpVector))
		);
		//UE_LOG(LogTemp, Warning, TEXT("Angle: %f"), Angle);
		bool bWalkable = Angle <= 45.f;            // walkable terrain// acceptable roof height

		if (bWalkable)
		{
			GetWorld()->LineTraceSingleByChannel(Hit, DownStart, DownEnd, ECC_Visibility, Params);
			//DrawDebugLine(GetWorld(),DownStart, DownEnd, FColor::Red, false);
			return Hit;
		}
	}
	// STEP 3 — INVALID surface → project downward
	DownStart = DesiredLocation;  

	GetWorld()->LineTraceSingleByChannel(Hit, DownStart, DownEnd, ECC_Visibility, Params);
	//DrawDebugLine(GetWorld(),DownStart, DownEnd, FColor::Green, false);
	return Hit;
}