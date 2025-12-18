#pragma once

#include "CoreMinimal.h"
#include "Widgets/SCompoundWidget.h"

class PROJ_API SLoadingScreen : public SCompoundWidget
{
public:
	SLATE_BEGIN_ARGS(SLoadingScreen){}
		SLATE_ARGUMENT(UTexture2D*, BackgroundTexture)
	SLATE_END_ARGS()

	/** Constructs this widget with InArgs */
	void Construct(const FArguments& InArgs);

private:
	FSlateBrush BackgroundBrush;
};
