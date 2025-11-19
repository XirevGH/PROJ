// Fill out your copyright notice in the Description page of Project Settings.


#include "Indicator.h"
#include "Components/DecalComponent.h"
#include "Components/SceneComponent.h"

// Sets default values
AIndicator::AIndicator()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.TickGroup = TG_PostUpdateWork;

	MaxRange = 999999.0f;
	
	Decal = CreateDefaultSubobject<UDecalComponent>(TEXT("Decal"));
	Root = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	SetRootComponent(Root);
	Decal->SetupAttachment(Root);

	
}

// Called when the game starts or when spawned
void AIndicator::BeginPlay()
{
	Super::BeginPlay();
	
}

FHitResult AIndicator::PerformTrace()
{
	float MouseX, MouseY;
	FHitResult Hit = FHitResult();
	if (!CasterController)
	{
		UE_LOG(LogTemp, Warning, TEXT("No APlayerController"));
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

// Called every frame
void AIndicator::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);	
	
	if (!IsHidden())
	{
		FHitResult HitResult = PerformTrace();
		FVector EndPoint = HitResult.Location;
		SetActorLocation(EndPoint);
	}
}

