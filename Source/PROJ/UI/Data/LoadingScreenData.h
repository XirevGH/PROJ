#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "Engine/Texture2D.h"
#include "LoadingScreenData.generated.h"

UCLASS(BlueprintType)
class PROJ_API ULoadingScreenData : public UDataAsset
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Backgrounds")
	TArray<UTexture2D*> Backgrounds;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Logo")
	UTexture2D* Logo;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Tips", meta = (MultiLine = true))
	TArray<FText> LoadingTips;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Tips")
	float TipSwitchInterval = 10.0f;
};