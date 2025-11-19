// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Indicator.generated.h"

UCLASS()
class PROJ_API AIndicator : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	AIndicator();
	

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UDecalComponent* Decal;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	USceneComponent* Root;

	UPROPERTY(BlueprintReadWrite)
	APlayerController* CasterController;

	UPROPERTY(BlueprintReadWrite)
	AActor* Caster;
	
	UPROPERTY(BlueprintReadWrite)
	float MaxRange;
	
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	FHitResult PerformTrace(); 

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;
};
