#pragma once

#include "CoreMinimal.h"
#include "Engine/DeveloperSettings.h"
#include "PROJ/UI/Data/LoadingScreenData.h"
#include "LoadingScreenSettings.generated.h"

/**
 * Registered in Project Settings -> Game -> Loading Screen
 */
UCLASS(Config=Game, defaultconfig, meta=(DisplayName="Loading Screen"))
class PROJ_API ULoadingScreenSettings : public UDeveloperSettings
{
	GENERATED_BODY()

public:
	// Reference to the Data Asset created earlier
	UPROPERTY(Config, EditAnywhere, Category = "Config")
	TSoftObjectPtr<ULoadingScreenData> LoadingScreenConfig;
};
