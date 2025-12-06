// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BasePlayerState.h"
#include "GameFramework/GameState.h"
#include "BaseGameState.generated.h"

/**
 * 
 */
UCLASS()
class PROJ_API ABaseGameState : public AGameState
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TMap<FString,TArray<ABasePlayerState*>> PlayerStateTeams;
};
