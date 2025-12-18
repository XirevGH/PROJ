#pragma once

#include "CoreMinimal.h"
#include "Data/LoadingScreenData.h"
#include "Widgets/SCompoundWidget.h"

class PROJ_API SLoadingScreen : public SCompoundWidget
{
public:
	SLATE_BEGIN_ARGS(SLoadingScreen){}
	SLATE_END_ARGS()

	/** Constructs this widget with InArgs */
	void Construct(const FArguments& InArgs);
	virtual void Tick(const FGeometry& AllottedGeometry, const double InCurrentTime, const float InDeltaTime) override;

private:
	FSlateBrush BackgroundBrush;
	FSlateBrush LogoBrush;

	TStrongObjectPtr<ULoadingScreenData> LoadedDataAsset; 

	TArray<FText> Tips;
	float TipInterval;
	float TimeSinceLastTipUpdate;
	TSharedPtr<STextBlock> TipTextBlock;
	void DisplayRandomTip();
};
