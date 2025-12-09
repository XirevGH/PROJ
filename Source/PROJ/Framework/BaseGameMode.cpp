// Fill out your copyright notice in the Description page of Project Settings.


#include "BaseGameMode.h"

UCharacterClassInfo* ABaseGameMode::GetCharacterClassDefaultInfo() const
{
	return ClassDefaults;
}

void ABaseGameMode::HandleSeamlessTravelPlayer(AController*& C)
{
	Super::HandleSeamlessTravelPlayer(C);
	UE_LOG(LogTemp, Warning, TEXT("Controller: %s"), *C->GetName());

	// 2. Check if the controller is a human player
	if (APlayerController* PC = Cast<APlayerController>(C))
	{
		// 3. Force the player to spawn a Pawn at a PlayerStart
		RestartPlayer(C);
	}
}
