// SLoadingScreen.cpp

#include "LoadingScreen.h"
#include "SlateOptMacros.h"
#include "Widgets/Layout/SScaleBox.h"
#include "Widgets/Layout/SBox.h"
#include "Widgets/Images/SThrobber.h"
#include "Engine/Engine.h"        // Needed for GEngine
#include "Engine/GameViewportClient.h" // Needed for Viewport size

BEGIN_SLATE_FUNCTION_BUILD_OPTIMIZATION

void SLoadingScreen::Construct(const FArguments& InArgs)
{
    SetVisibility(EVisibility::HitTestInvisible);

    // --- 1. Fetch Data ---
    UTexture2D* BGTexture = InArgs._SelectedBackground;
    UTexture2D* LogoTexture = InArgs._SelectedLogo;
    Tips = InArgs._LoadingTips;
    TipInterval = 5.0f;
    
    BGTextureSize = FVector2D(1920, 1080); // Default

    // --- 2. Setup Brushes ---
    if (BGTexture && BGTexture->IsValidLowLevel())
    {
        BGTextureSize = FVector2D(BGTexture->GetSizeX(), BGTexture->GetSizeY());
        BackgroundBrush.SetResourceObject(BGTexture);
        BackgroundBrush.ImageSize = BGTextureSize;
        BackgroundBrush.DrawAs = ESlateBrushDrawType::Image;
        BackgroundBrush.TintColor = FLinearColor::White;
    }
    else
    {
        BackgroundBrush.DrawAs = ESlateBrushDrawType::NoDrawType;
    }

    if (LogoTexture && LogoTexture->IsValidLowLevel())
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
            SAssignNew(BackgroundScaleBox, SScaleBox)
            .Stretch(EStretch::UserSpecified) // Manual Scaling
            [
                SNew(SImage).Image(&BackgroundBrush)
            ]
        ]

        // Layer 2: Logo (Constrained)
        + SOverlay::Slot()
        .HAlign(HAlign_Left)
        .VAlign(VAlign_Top)
        .Padding(60.f)
        [
            SNew(SBox)
            .WidthOverride(500.0f)
            .HeightOverride(250.0f)
            [
                SNew(SScaleBox)
                .Stretch(EStretch::ScaleToFit)
                .HAlign(HAlign_Left)
                .VAlign(VAlign_Top)
                [
                    SNew(SImage).Image(&LogoBrush)
                ]
            ]
        ]

        // Layer 3: Footer
        + SOverlay::Slot()
        .HAlign(HAlign_Fill)
        .VAlign(VAlign_Bottom)
        [
            SNew(SBox)
            .HeightOverride(200.0f)
            .Clipping(EWidgetClipping::ClipToBounds) 
            [
                SNew(SOverlay)
                
                + SOverlay::Slot()
                [
                    SNew(SImage).ColorAndOpacity(FLinearColor(0,0,0,0.85f))
                ]

                + SOverlay::Slot()
                .Padding(FMargin(60.f, 0.f))
                [
                    SNew(SHorizontalBox)

                    // Tip Text
                    + SHorizontalBox::Slot()
                    .FillWidth(0.7f)
                    .HAlign(HAlign_Left)
                    .VAlign(VAlign_Center)
                    [
                        SAssignNew(TipTextBlock, STextBlock)
                        .Font(FCoreStyle::GetDefaultFontStyle("Regular", 18))
                        .ColorAndOpacity(FLinearColor(0.9f, 0.9f, 0.9f, 1.0f))
                        .AutoWrapText(true)
                        .WrapTextAt(1000.0f)
                    ]

                    // Spinner
                    + SHorizontalBox::Slot()
                    .FillWidth(0.3f)
                    .HAlign(HAlign_Right)
                    .VAlign(VAlign_Center)
                    [
                        SNew(SHorizontalBox)
                        + SHorizontalBox::Slot().AutoWidth().Padding(0,0,20,0).VAlign(VAlign_Center)
                        [
                            SNew(STextBlock).Text(FText::FromString("LOADING..."))
                            .Font(FCoreStyle::GetDefaultFontStyle("Bold", 24))
                        ]
                        + SHorizontalBox::Slot().AutoWidth().VAlign(VAlign_Center)
                        [
                            SNew(SCircularThrobber).Radius(20.0f)
                        ]
                    ]
                ]
            ]
        ]
    ];
    
    // --- 4. PRE-CALCULATE SCALE (FIX FOR GLITCH) ---
    // This runs BEFORE the first frame is drawn.
    if (BackgroundScaleBox.IsValid() && BGTextureSize.X > 0 && BGTextureSize.Y > 0)
    {
        FVector2D ViewportSize = FVector2D(1920, 1080); // Default fallback

        // Try to get the actual viewport size immediately
        if (GEngine && GEngine->GameViewport)
        {
            FVector2D ActualSize;
            GEngine->GameViewport->GetViewportSize(ActualSize);
            if (ActualSize.X > 0 && ActualSize.Y > 0)
            {
                ViewportSize = ActualSize;
            }
        }

        float ScaleX = ViewportSize.X / BGTextureSize.X;
        float ScaleY = ViewportSize.Y / BGTextureSize.Y;
        
        // Apply "Cover" scale immediately
        BackgroundScaleBox->SetUserSpecifiedScale(FMath::Max(ScaleX, ScaleY));
    }

    // --- 5. Set Initial Tip ---
    if (Tips.Num() > 0 && TipTextBlock.IsValid())
    {
        int32 SafeIndex = FMath::Clamp(InArgs._InitialTipIndex, 0, Tips.Num() - 1);
        TipTextBlock->SetText(Tips[SafeIndex]);
    }
}

void SLoadingScreen::Tick(const FGeometry& AllottedGeometry, const double InCurrentTime, const float InDeltaTime)
{
    SCompoundWidget::Tick(AllottedGeometry, InCurrentTime, InDeltaTime);

    // Background Scaling
    if (BackgroundScaleBox.IsValid() && BGTextureSize.X > 0 && BGTextureSize.Y > 0)
    {
        const FVector2D ScreenSize = AllottedGeometry.GetLocalSize();
        if (ScreenSize.X > 0 && ScreenSize.Y > 0)
        {
            float ScaleX = ScreenSize.X / BGTextureSize.X;
            float ScaleY = ScreenSize.Y / BGTextureSize.Y;
            BackgroundScaleBox->SetUserSpecifiedScale(FMath::Max(ScaleX, ScaleY));
        }
    }

    // Tip Cycling
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