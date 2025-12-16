// Fill out your copyright notice in the Description page of Project Settings.


#include "LoadingScreen.h"

#include "SlateOptMacros.h"
#include "Widgets/Layout/SScaleBox.h"
#include "Widgets/Images/SThrobber.h"

BEGIN_SLATE_FUNCTION_BUILD_OPTIMIZATION

void SLoadingScreen::Construct(const FArguments& InArgs)
{
    SetVisibility(EVisibility::HitTestInvisible);
    
    // 1. Set up the Brush (Image container)
    if (InArgs._BackgroundTexture)
    {
        // Point the brush to the texture resource
        BackgroundBrush.SetResourceObject(InArgs._BackgroundTexture);
        BackgroundBrush.ImageSize = FVector2D(InArgs._BackgroundTexture->GetSizeX(), InArgs._BackgroundTexture->GetSizeY());
        BackgroundBrush.DrawAs = ESlateBrushDrawType::Image;
        BackgroundBrush.TintColor = FLinearColor::White;
    }
    else
    {
        // Fallback to black if no texture found
        BackgroundBrush.DrawAs = ESlateBrushDrawType::NoDrawType; 
    }

    ChildSlot
    [
        SNew(SOverlay)
        
        // LAYER 1: The Background Image
        + SOverlay::Slot()
        .HAlign(HAlign_Fill)
        .VAlign(VAlign_Fill)
        [
            // SScaleBox handles "Aspect Ratio" logic (Like "Size to Box" in UMG)
            SNew(SScaleBox)
            .Stretch(EStretch::ScaleToFill) // "ScaleToFill" cuts off edges to fill screen (Zoom/Cover)
            [
                SNew(SImage)
                .Image(&BackgroundBrush)
            ]
        ]

        // LAYER 2: A dark overlay to make text readable (Optional)
        + SOverlay::Slot()
        .HAlign(HAlign_Fill)
        .VAlign(VAlign_Bottom)
        [
            SNew(SBox)
            .HeightOverride(200.0f)
            [
                SNew(SImage)
                .ColorAndOpacity(FLinearColor(0, 0, 0, 0.5f)) // 50% transparent black
            ]
        ]

        // LAYER 3: The Text and Spinner (Bottom Right)
        + SOverlay::Slot()
        .HAlign(HAlign_Right)
        .VAlign(VAlign_Bottom)
        .Padding(50.0f)
        [
            SNew(SHorizontalBox)
            + SHorizontalBox::Slot()
            .AutoWidth()
            .VAlign(VAlign_Center)
            .Padding(0, 0, 20, 0)
            [
                SNew(STextBlock)
                .Text(FText::FromString("LOADING..."))
                .Font(FCoreStyle::GetDefaultFontStyle("Regular", 24))
            ]
            + SHorizontalBox::Slot()
            .AutoWidth()
            .VAlign(VAlign_Center)
            [
                SNew(SCircularThrobber)
                .Radius(20.0f)
                .NumPieces(8)
            ]
        ]
    ];
}

END_SLATE_FUNCTION_BUILD_OPTIMIZATION
