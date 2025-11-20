// Fill out your copyright notice in the Description page of Project Settings.


#include "GATA_GroundTrace_Indicator.h"
#include "Components/DecalComponent.h"
#include "Components/SceneComponent.h"
#include "Abilities/GameplayAbility.h"
#include "GameFramework/Actor.h"

AGATA_GroundTrace_Indicator::AGATA_GroundTrace_Indicator()
{
	Decal = CreateDefaultSubobject<UDecalComponent>(TEXT("Decal"));
	Root = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	SetRootComponent(Root);
	Decal->SetupAttachment(Root);
	Decal->SetRelativeLocation(FVector(0, 0, 0));
}


FHitResult AGATA_GroundTrace_Indicator::PerformTrace(AActor* InSourceActor)
{
	float MouseX, MouseY;
	FHitResult Hit = FHitResult();
	APawn* Pawn = Cast<APawn>(InSourceActor);
	APlayerController* CasterController = Pawn ? Cast<APlayerController>(Pawn->GetController()) : nullptr;
	if (!CasterController)
	{
		UE_LOG(LogTemp, Error, TEXT("No APlayerController"));
		return Hit;
	}
	
	if (!CasterController->GetMousePosition(MouseX, MouseY))
	{
		UE_LOG(LogTemp, Warning, TEXT("Cant GetMousePosition"));
		return Hit;
	}

	// Convert to World Space
	FVector WorldOrigin;
	FVector WorldDirection;
	if (!CasterController->DeprojectScreenPositionToWorld(MouseX, MouseY, WorldOrigin, WorldDirection))
	{
		UE_LOG(LogTemp, Warning, TEXT("Cant DeprojectScreenPositionToWorld"));
		return Hit;
	}
	FVector TraceStart = WorldOrigin;
	FVector TraceEnd = TraceStart + WorldDirection * MaxRange; // trace far enough

	//DrawDebugLine(GetWorld(),TraceStart, TraceEnd, FColor::Red, true, 0, 0, 0);

	FCollisionQueryParams Params;
	Params.AddIgnoredActor(this); // ignore self

	GetWorld()->LineTraceSingleByChannel(Hit, TraceStart, TraceEnd, ECC_Visibility, Params);
	return Hit;
}
