#include "LoadingScreen.h"
#include "SlateOptMacros.h"
#include "Widgets/Layout/SScaleBox.h"
#include "Widgets/Layout/SBox.h"
#include "Widgets/Images/SThrobber.h"

BEGIN_SLATE_FUNCTION_BUILD_OPTIMIZATION

void SLoadingScreen::Construct(const FArguments& InArgs)
{
    SetVisibility(EVisibility::HitTestInvisible);

    // --- 1. Fetch Data ---
    UTexture2D* BGTexture = InArgs._SelectedBackground;
    if (BGTexture && BGTexture->IsValidLowLevel())
    {
        BGTextureSize = FVector2D(BGTexture->GetSizeX(), BGTexture->GetSizeY());
        BackgroundBrush.SetResourceObject(BGTexture);
        // ... rest of settings
    }
    else
    {
        BackgroundBrush.DrawAs = ESlateBrushDrawType::NoDrawType;
    }
    
    UTexture2D* LogoTexture = InArgs._SelectedLogo;
    
    // CHANGED: Direct assignment (FText array)
    Tips = InArgs._LoadingTips; 
    
    TipInterval = 5.0f;
    BGTextureSize = FVector2D(1920, 1080); 

    // --- 2. Setup Brushes ---
    if (BGTexture)
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

        // Layer 1: Background (CSS Cover Mode)
        + SOverlay::Slot()
        .HAlign(HAlign_Fill)
        .VAlign(VAlign_Fill)
        [
            SAssignNew(BackgroundScaleBox, SScaleBox)
            .Stretch(EStretch::UserSpecified)
            [
                SNew(SImage).Image(&BackgroundBrush)
            ]
        ]

        // Layer 2: Logo (Constrained Size)
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

        // Layer 3: Footer (Locked Height + Clipping)
        + SOverlay::Slot()
        .HAlign(HAlign_Fill)
        .VAlign(VAlign_Bottom)
        [
            SNew(SBox)
            .HeightOverride(200.0f)
            .Clipping(EWidgetClipping::ClipToBounds) // Fixes the "Big Black Box" glitch
            [
                SNew(SOverlay)
                
                // Black transparent background
                + SOverlay::Slot()
                [
                    SNew(SImage).ColorAndOpacity(FLinearColor(0,0,0,0.85f))
                ]

                // Content
                + SOverlay::Slot()
                .Padding(FMargin(60.f, 0.f))
                [
                    SNew(SHorizontalBox)

                    // Tip Text (Left)
                    + SHorizontalBox::Slot()
                    .FillWidth(0.7f)
                    .HAlign(HAlign_Left)
                    .VAlign(VAlign_Center)
                    [
                        SAssignNew(TipTextBlock, STextBlock)
                        .Font(FCoreStyle::GetDefaultFontStyle("Regular", 18))
                        .ColorAndOpacity(FLinearColor(0.9f, 0.9f, 0.9f, 1.0f))
                        .AutoWrapText(true)
                        .WrapTextAt(1000.0f) // Helps prevent layout jitter
                    ]

                    // Spinner (Right)
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
    
    DisplayRandomTip();
}

void SLoadingScreen::Tick(const FGeometry& AllottedGeometry, const double InCurrentTime, const float InDeltaTime)
{
    SCompoundWidget::Tick(AllottedGeometry, InCurrentTime, InDeltaTime);

    // Background Aspect Ratio Logic
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

    // Tip Switch Logic
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
        // CHANGED: No longer FText::FromString(), just pass the FText directly
        TipTextBlock->SetText(Tips[FMath::RandRange(0, Tips.Num() - 1)]);
    }
}

END_SLATE_FUNCTION_BUILD_OPTIMIZATION