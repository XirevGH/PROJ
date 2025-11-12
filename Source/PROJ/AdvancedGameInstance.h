// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AdvancedFriendsGameInstance.h"
#include "AdvancedGameInstance.generated.h"

/**
 * 
 */
UCLASS()
class PROJ_API UAdvancedGameInstance : public UAdvancedFriendsGameInstance
{
	GENERATED_BODY()

public:
	UAdvancedGameInstance(const FObjectInitializer& ObjectInitializer);

protected:
	virtual void Init() override;

	UFUNCTION(BlueprintImplementableEvent, Category = "AdvancedFriends")
	void OnDestroySessionComplete(FName SessionName, bool bSuccess);

private:
	FOnDestroySessionCompleteDelegate OnDestroySessionCompleteDelegate;
};
