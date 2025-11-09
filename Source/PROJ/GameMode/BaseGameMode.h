// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameMode.h"
#include "BaseGameMode.generated.h"


class UCharacterClassInfo;
/**
 * 
 */
UCLASS()
class PROJ_API ABaseGameMode : public AGameMode
{
	GENERATED_BODY()

public:
	UFUNCTION()
	UCharacterClassInfo* GetCharacterClassDefaultInfo() const;
private:
	
	UPROPERTY(EditDefaultsOnly, Category = "Custom Values|Class Defaults")
	TObjectPtr<UCharacterClassInfo> ClassDefaults;
};
