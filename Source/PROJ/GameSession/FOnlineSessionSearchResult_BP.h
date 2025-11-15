#pragma once

#include "CoreMinimal.h"
#include "FOnlineSessionSearchResult_BP.generated.h"

USTRUCT(BlueprintType)
struct PROJ_API FOnlineSessionSearchResult_BP
{
	GENERATED_BODY()
	
	UPROPERTY(BlueprintReadOnly)
	FString SessionName;

	UPROPERTY(BlueprintReadOnly)
	FString GameMode;

	UPROPERTY(BlueprintReadOnly)
	int32 CurrentPlayers = 0;

	UPROPERTY(BlueprintReadOnly)
	int32 MaxPlayers = 0;

	UPROPERTY(BlueprintReadOnly)
	int32 Ping = 0;
};
