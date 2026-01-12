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
	
	// trace from camera in mouse direction
	FVector CameraTraceEnd = WorldOrigin + WorldDirection * 100000.f;
	FCollisionQueryParams Params;
	Params.AddIgnoredActor(this);
	Params.bTraceComplex = true;
	
	GetWorld()->LineTraceSingleByChannel(Hit, WorldOrigin, CameraTraceEnd, ECC_Visibility, Params);
	
	if (!Hit.bBlockingHit)
	{
		return Hit;
	}
	
	FVector MouseHitLocation = Hit.Location;
	
	//Clamp to ability range
	FVector PlayerPos = InSourceActor->GetActorLocation();
	FVector VectorToHit = MouseHitLocation - PlayerPos;
	float DistanceToHit = VectorToHit.Size();
	FVector Direction = VectorToHit.GetSafeNormal();
	
	if (DistanceToHit  > MaxRange)
	{
		MouseHitLocation = PlayerPos + Direction * MaxRange;
	}

	//Check if the hit angle is walkable
	float Angle = FMath::RadiansToDegrees(
		acosf(FVector::DotProduct(Hit.ImpactNormal, FVector::UpVector))
	);
	//UE_LOG(LogTemp, Warning, TEXT("Angle: %f"), Angle);
	bool bWalkable = Angle <= 45.f; // walkable terrain

	//project down for the ground position with some height offset to prevent the trace from going through the mesh 
	FVector DownStart = MouseHitLocation + FVector(0,0,10); // lift a little for safety
	FVector DownEnd = MouseHitLocation - FVector(0,0,5000);

	if (bWalkable)
	{
		GetWorld()->LineTraceSingleByChannel(Hit, DownStart, DownEnd, ECC_Visibility, Params);
		//DrawDebugLine(GetWorld(),DownStart, DownEnd, FColor::Red, false);
		return Hit;
	}

	//This does not work with stairs/ mesh with complex shape if the WallPushBackDistance = radius of the ability
	
	// If surface hit was too steep
	float WallPushBackDistance =  10;
	FVector Direction_XY = FVector(VectorToHit.X, VectorToHit.Y, 0.f).GetSafeNormal();
	FVector AdjustedLocation = MouseHitLocation + -Direction_XY * WallPushBackDistance;
	DownStart = AdjustedLocation; 
	DownEnd = AdjustedLocation - FVector(0,0,5000);
	
	GetWorld()->LineTraceSingleByChannel(Hit, DownStart, DownEnd, ECC_Visibility, Params);
	//DrawDebugLine(GetWorld(),MouseHitLocation, AdjustedLocation, FColor::Green, false);
	return Hit;
}