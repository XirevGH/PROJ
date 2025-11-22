// Fill out your copyright notice in the Description page of Project Settings.


#include "BaseAbilitySystemLibrary.h"

#include "Kismet/GameplayStatics.h"
#include "PROJ/Framework/BaseGameMode.h"

UCharacterClassInfo* UBaseAbilitySystemLibrary::GetCharacterClassDefaultInfo(const UObject* WorldContextObject)
{
	if (const ABaseGameMode* GameMode = Cast<ABaseGameMode>(UGameplayStatics::GetGameMode(WorldContextObject)))
	{
		return GameMode->GetCharacterClassDefaultInfo();
	}
	return nullptr;
}
