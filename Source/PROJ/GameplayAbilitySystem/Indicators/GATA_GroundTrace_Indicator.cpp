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

	// Clamp the trace end to MaxRange from ability origin
	FVector AbilityOrigin = InSourceActor->GetActorLocation();
	FVector TraceEnd = WorldOrigin + WorldDirection * MaxRange;

	float DistanceToOrigin = FVector::Dist(AbilityOrigin, TraceEnd);
	if (DistanceToOrigin > MaxRange)
	{
		FVector Direction = (TraceEnd - AbilityOrigin).GetSafeNormal();
		TraceEnd = AbilityOrigin + Direction * MaxRange;
	}

	// Perform the trace
	FCollisionQueryParams Params;
	Params.AddIgnoredActor(this);
	GetWorld()->LineTraceSingleByChannel(Hit, WorldOrigin, TraceEnd, ECC_Visibility, Params);

	return Hit;
}