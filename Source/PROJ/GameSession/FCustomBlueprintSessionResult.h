#pragma once

#include "CoreMinimal.h"
#include "BlueprintDataDefinitions.h"
#include "FindSessionsCallbackProxy.h"
#include "FCustomBlueprintSessionResult.generated.h"

USTRUCT(BlueprintType)
struct PROJ_API FCustomBlueprintSessionResult
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite)
	FBlueprintSessionResult SessionResult;

	UPROPERTY(BlueprintReadWrite)
	FBPUniqueNetId HostId;
};
