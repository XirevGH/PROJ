#pragma once

#include "CoreMinimal.h"
#include "Data/LoadingScreenData.h"
#include "Widgets/SCompoundWidget.h"

class SScaleBox;

class PROJ_API SLoadingScreen : public SCompoundWidget
{
public:
	SLATE_BEGIN_ARGS(SLoadingScreen)
		: _SelectedBackground(nullptr)
		, _SelectedLogo(nullptr)
	{}
		SLATE_ARGUMENT(UTexture2D*, SelectedBackground)
		SLATE_ARGUMENT(UTexture2D*, SelectedLogo)
		SLATE_ARGUMENT(TArray<FText>, LoadingTips)
	SLATE_END_ARGS()

	/** Constructs this widget with InArgs */
	void Construct(const FArguments& InArgs);
	virtual void Tick(const FGeometry& AllottedGeometry, const double InCurrentTime, const float InDeltaTime) override;

private:
	FSlateBrush BackgroundBrush;
	FSlateBrush LogoBrush;

	FVector2D BGTextureSize = FVector2D::ZeroVector;
	TSharedPtr<SScaleBox> BackgroundScaleBox; 

	TStrongObjectPtr<ULoadingScreenData> LoadedDataAsset; 

	TArray<FText> Tips;
	float TipInterval = 0;
	float TimeSinceLastTipUpdate = 0;
	TSharedPtr<STextBlock> TipTextBlock;
	void DisplayRandomTip();
};
