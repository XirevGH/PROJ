// Fill out your copyright notice in the Description page of Project Settings.


#include "GATA_GroundTrace_Indicator.h"
#include "Components/DecalComponent.h"
#include "Components/SceneComponent.h"

AGATA_GroundTrace_Indicator::AGATA_GroundTrace_Indicator()
{
	Decal = CreateDefaultSubobject<UDecalComponent>(TEXT("Decal"));
	Root = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	SetRootComponent(Root);
	Decal->SetupAttachment(Root);

	
	
}
