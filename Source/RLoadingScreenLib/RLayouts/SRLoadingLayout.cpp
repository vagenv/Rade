// Copyright 2015-2023 Vagen Ayrapetyan


#include "SRLoadingLayout.h"
#include "../RLoadingScreenSettings.h"
#include "Widgets/Layout/SSafeZone.h"
#include "Widgets/Layout/SDPIScaler.h"
#include "Widgets/Layout/SSpacer.h"
#include "Widgets/SBoxPanel.h"

#include "../RWidgets/SHorizontalLoadingWidget.h"
#include "../RWidgets/SVerticalLoadingWidget.h"
#include "../RWidgets/SBackgroundWidget.h"
#include "../RWidgets/STipWidget.h"
#include "../RWidgets/SLoadingCompleteText.h"

void SRLoadingLayout::Construct (
   const FArguments               &InArgs,
   const FALoadingScreenSettings &ScreenSettings,
   const FLoadingLayoutSettings  &LayoutSettings)
{
   // Root widget and background
   TSharedRef<SOverlay> Root = SNew(SOverlay)
      + SOverlay::Slot()
      .HAlign(HAlign_Fill)
      .VAlign(VAlign_Fill)
      [
         SNew(SBackgroundWidget, ScreenSettings.Background)
      ];

   // Placeholder for loading widget
   TSharedRef<SWidget> LoadingWidget = SNullWidget::NullWidget;
   if (ScreenSettings.LoadingWidget.LoadingWidgetType == ELoadingWidgetType::LWT_Horizontal) {
      LoadingWidget = SNew(SHorizontalLoadingWidget, ScreenSettings.LoadingWidget);
   } else {
      LoadingWidget = SNew(SVerticalLoadingWidget, ScreenSettings.LoadingWidget);
   }

   TSharedRef<SHorizontalBox> HorizontalBox = SNew(SHorizontalBox);

   if (LayoutSettings.bIsLoadingWidgetAtLeft) {
      // Add Loading widget on left first
      HorizontalBox.Get().AddSlot()
         .VAlign(VAlign_Center)
         .HAlign(HAlign_Center)
         .AutoWidth()
         [
            LoadingWidget
         ];

      // Add spacer at midder
      HorizontalBox.Get().AddSlot()
         .HAlign(HAlign_Fill)
         .VAlign(VAlign_Fill)
         .AutoWidth()
         [
            SNew(SSpacer)
            .Size(FVector2D(LayoutSettings.Space, 0.0f))
         ];

         // Tip Text on the right
      HorizontalBox.Get().AddSlot()
         .FillWidth(1.0f)
         .HAlign(LayoutSettings.TipAlignment.HorizontalAlignment)
         .VAlign(LayoutSettings.TipAlignment.VerticalAlignment)
         [
            SNew(STipWidget, ScreenSettings.TipWidget)
         ];

   } else {

      // Tip Text on the left
      HorizontalBox.Get().AddSlot()
         .FillWidth(1.0f)
         .HAlign(LayoutSettings.TipAlignment.HorizontalAlignment)
         .VAlign(LayoutSettings.TipAlignment.VerticalAlignment)
         [
            // Add tip text
            SNew(STipWidget, ScreenSettings.TipWidget)
         ];

      // Add spacer at midder
      HorizontalBox.Get().AddSlot()
         .HAlign(HAlign_Fill)
         .VAlign(VAlign_Fill)
         .AutoWidth()
         [
            SNew(SSpacer)
            .Size(FVector2D(LayoutSettings.Space, 0.0f))
         ];

      // Add Loading widget on right
      HorizontalBox.Get().AddSlot()
         .VAlign(VAlign_Center)
         .HAlign(HAlign_Center)
         .AutoWidth()
         [
            LoadingWidget
         ];
   }

   EVerticalAlignment VerticalAlignment;
   // Set vertical alignment for widget
   if (LayoutSettings.bIsWidgetAtBottom) {
      VerticalAlignment = EVerticalAlignment::VAlign_Bottom;
   } else {
      VerticalAlignment = EVerticalAlignment::VAlign_Top;
   }

   // Creating loading theme
   Root->AddSlot()
   .HAlign(LayoutSettings.BorderHorizontalAlignment)
   .VAlign(VerticalAlignment)
   [
      SNew(SBorder)
      .HAlign(HAlign_Fill)
      .VAlign(VAlign_Fill)
      .BorderImage(&LayoutSettings.BorderBackground)
      .BorderBackgroundColor(FLinearColor::White)
      [
         SNew(SSafeZone)
         .HAlign(HAlign_Fill)
         .VAlign(VAlign_Fill)
         .IsTitleSafe(true)
         .Padding(LayoutSettings.BorderPadding)
         [
            SNew(SDPIScaler)
            .DPIScale(this, &SRLoadingLayout::GetDPIScale)
            [
               HorizontalBox
            ]
         ]
      ]
   ];

   // Construct loading complete text if enable
   if (ScreenSettings.bShowLoadingCompleteText) {
      Root->AddSlot()
         .VAlign(ScreenSettings.LoadingCompleteTextSettings.Alignment.VerticalAlignment)
         .HAlign(ScreenSettings.LoadingCompleteTextSettings.Alignment.HorizontalAlignment)
         .Padding(ScreenSettings.LoadingCompleteTextSettings.Padding)
         [
            SNew(SLoadingCompleteText, ScreenSettings.LoadingCompleteTextSettings)
         ];
   }

   // Add root to this widget
   ChildSlot
   [
      Root
   ];
}

