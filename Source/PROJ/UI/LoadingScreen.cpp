// Fill out your copyright notice in the Description page of Project Settings.


#include "LoadingScreen.h"

#include "SlateOptMacros.h"
#include "ProjectSettings/LoadingScreenSettings.h"
#include "Widgets/Layout/SScaleBox.h"
#include "Widgets/Images/SThrobber.h"

BEGIN_SLATE_FUNCTION_BUILD_OPTIMIZATION

void SLoadingScreen::Construct(const FArguments& InArgs)
{
    SetVisibility(EVisibility::HitTestInvisible);

    // --- 1. Fetch Data from Project Settings ---
    Tips.Empty();
    TipInterval = 5.0f;
    LoadedDataAsset.Reset();
	
    // Get the settings singleton
    const ULoadingScreenSettings* Settings = GetDefault<ULoadingScreenSettings>();
	
    if (Settings && !Settings->LoadingScreenConfig.IsNull())
    {
        // Load the Data Asset Synchronously (It's a loading screen, blocking is fine here)
    	LoadedDataAsset.Reset(Settings->LoadingScreenConfig.LoadSynchronous());
    }

    UTexture2D* BGTexture = nullptr;
    UTexture2D* LogoTexture = nullptr;

    if (LoadedDataAsset.IsValid())
    {
        // Extract Data
        if (LoadedDataAsset->Backgrounds.Num() > 0)
        {
            BGTexture = LoadedDataAsset->Backgrounds[FMath::RandRange(0, LoadedDataAsset->Backgrounds.Num() - 1)];
        }
		
        LogoTexture = LoadedDataAsset->Logo;
        Tips = LoadedDataAsset->LoadingTips;
        TipInterval = LoadedDataAsset->TipSwitchInterval;
    }
    
   // --- 2. Setup Brushes ---
	if (BGTexture)
	{
		BackgroundBrush.SetResourceObject(BGTexture);
		BackgroundBrush.ImageSize = FVector2D(BGTexture->GetSizeX(), BGTexture->GetSizeY());
		BackgroundBrush.DrawAs = ESlateBrushDrawType::Image;
		BackgroundBrush.TintColor = FLinearColor::White;
	}
	else
	{
		BackgroundBrush.DrawAs = ESlateBrushDrawType::NoDrawType;
	}

	if (LogoTexture)
	{
		LogoBrush.SetResourceObject(LogoTexture);
		LogoBrush.ImageSize = FVector2D(LogoTexture->GetSizeX(), LogoTexture->GetSizeY());
		LogoBrush.DrawAs = ESlateBrushDrawType::Image;
		LogoBrush.TintColor = FLinearColor::White;
	}
	else
	{
		LogoBrush.DrawAs = ESlateBrushDrawType::NoDrawType;
	}

	// --- 3. Build UI Layout ---
	ChildSlot
	[
		SNew(SOverlay)

		// Layer 1: Background
		+ SOverlay::Slot()
		.HAlign(HAlign_Fill)
		.VAlign(VAlign_Fill)
		[
			SNew(SScaleBox)
			.Stretch(EStretch::ScaleToFill)
			[
				SNew(SImage).Image(&BackgroundBrush)
			]
		]

		// Layer 2: Logo (Top Left)
		+ SOverlay::Slot()
		.HAlign(HAlign_Left)
		.VAlign(VAlign_Top)
		.Padding(40.f)
		[
			SNew(SImage).Image(&LogoBrush)
		]

		// Layer 3: Footer Black Box
		+ SOverlay::Slot()
		.HAlign(HAlign_Fill)
		.VAlign(VAlign_Bottom)
		[
			SNew(SBox)
			.HeightOverride(200.0f)
			[
				SNew(SOverlay)
				
				// Black transparent background
				+ SOverlay::Slot()
				[
					SNew(SImage).ColorAndOpacity(FLinearColor(0,0,0,0.75f))
				]

				// Content
				+ SOverlay::Slot()
				.Padding(40.f)
				[
					SNew(SHorizontalBox)

					// Tip Text (Left)
					+ SHorizontalBox::Slot()
					.FillWidth(0.5f)
					.HAlign(HAlign_Left)
					.VAlign(VAlign_Top)
					[
						SAssignNew(TipTextBlock, STextBlock)
						.Font(FCoreStyle::GetDefaultFontStyle("Regular", 18))
						.AutoWrapText(true)
					]

					// Spinner (Right)
					+ SHorizontalBox::Slot()
					.FillWidth(0.5f)
					.HAlign(HAlign_Right)
					.VAlign(VAlign_Bottom)
					[
						SNew(SHorizontalBox)
						+ SHorizontalBox::Slot().AutoWidth().Padding(0,0,20,0).VAlign(VAlign_Center)
						[
							SNew(STextBlock).Text(FText::FromString("LOADING..."))
							.Font(FCoreStyle::GetDefaultFontStyle("Bold", 24))
						]
						+ SHorizontalBox::Slot().AutoWidth().VAlign(VAlign_Center)
						[
							SNew(SCircularThrobber)
						]
					]
				]
			]
		]
	];
	DisplayRandomTip();
}

void SLoadingScreen::Tick(const FGeometry& AllottedGeometry, const double InCurrentTime, const float InDeltaTime)
{
    SCompoundWidget::Tick(AllottedGeometry, InCurrentTime, InDeltaTime);

	if (Tips.Num() > 1)
	{
		TimeSinceLastTipUpdate += InDeltaTime;
		if (TimeSinceLastTipUpdate >= TipInterval)
		{
			DisplayRandomTip();
			TimeSinceLastTipUpdate = 0.0f;
		}
	}
}

void SLoadingScreen::DisplayRandomTip()
{
	if (Tips.Num() > 0 && TipTextBlock.IsValid())
	{
		TipTextBlock->SetText(Tips[FMath::RandRange(0, Tips.Num() - 1)]);
	}
}

END_SLATE_FUNCTION_BUILD_OPTIMIZATION
