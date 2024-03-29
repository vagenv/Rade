// Copyright 2015-2023 Vagen Ayrapetyan

#include "RSubStats_Property.h"
#include "RStatusLib/RStatusTypes.h"

#include "IDetailChildrenBuilder.h"
#include "SlateBasics.h"
#include "DetailLayoutBuilder.h"
#include "DetailWidgetRow.h"
#include "PropertyHandle.h"
#include "EditorStyleSet.h"

#define LOCTEXT_NAMESPACE "RTypesEditor"

TSharedRef<IPropertyTypeCustomization> FRSubStats_Property::MakeInstance ()
{
   return MakeShareable (new FRSubStats_Property ());
}

void FRSubStats_Property::CustomizeHeader (
   TSharedRef<IPropertyHandle>      StructPropertyHandle,
   FDetailWidgetRow&                  HeaderRow,
   IPropertyTypeCustomizationUtils& StructCustomizationUtils)
{
   // Don't show header if not an array item
   if (StructPropertyHandle->GetIndexInArray () != INDEX_NONE) {
      HeaderRow.NameContent ()[StructPropertyHandle->CreatePropertyNameWidget()];
   }
}

void FRSubStats_Property::CustomizeChildren (
   TSharedRef<IPropertyHandle>      StructPropertyHandle,
   IDetailChildrenBuilder&          StructBuilder,
   IPropertyTypeCustomizationUtils& StructCustomizationUtils)
{
   // --- Get handles
   TSharedPtr<IPropertyHandle> EvasionPropertyHandle
      = StructPropertyHandle->GetChildHandle(GET_MEMBER_NAME_CHECKED(FRSubStats, Evasion));

   TSharedPtr<IPropertyHandle> CriticalPropertyHandle
      = StructPropertyHandle->GetChildHandle(GET_MEMBER_NAME_CHECKED(FRSubStats, Critical));

   TSharedPtr<IPropertyHandle> MoveSpeedPropertyHandle
      = StructPropertyHandle->GetChildHandle(GET_MEMBER_NAME_CHECKED(FRSubStats, MoveSpeed));

   TSharedPtr<IPropertyHandle> AttackSpeedPropertyHandle
      = StructPropertyHandle->GetChildHandle(GET_MEMBER_NAME_CHECKED(FRSubStats, AttackSpeed));

   TSharedPtr<IPropertyHandle> AttackPowerPropertyHandle
      = StructPropertyHandle->GetChildHandle(GET_MEMBER_NAME_CHECKED(FRSubStats, AttackPower));


   // --- Perform checks
   check (  EvasionPropertyHandle.IsValid ()
         && CriticalPropertyHandle.IsValid ()
         && MoveSpeedPropertyHandle.IsValid ()
         && AttackSpeedPropertyHandle.IsValid ()
         && AttackPowerPropertyHandle.IsValid ());

   const float MinWidth = 70.f;
   const float HPadding = 10.f;
   bool IsInArray = StructPropertyHandle->GetIndexInArray () != INDEX_NONE;

   // Draw
   StructBuilder.AddCustomRow(LOCTEXT("FSubStatsRow", "FSubStats_Proper"))
   [
      // ROOT
      SNew(SBorder)
      .BorderImage(FAppStyle::GetBrush("ToolPanel.GroupBorder"))
      .Content()
      [
         SNew(SVerticalBox)
         +SVerticalBox::Slot()
         .AutoHeight()
         .HAlign (EHorizontalAlignment::HAlign_Center)
         [
            SNew(STextBlock)
            .Visibility_Lambda ([this, IsInArray] { return IsInArray ? EVisibility::Collapsed : EVisibility::Visible; })
            .Text (StructPropertyHandle->GetPropertyDisplayName ())
         ]
         +SVerticalBox::Slot()
         .AutoHeight()
         [

            // Wrap content
            SNew(SWrapBox)
            .UseAllottedWidth(true)

            // Evasion
            +SWrapBox::Slot()
            .Padding(HPadding, 0.f)
            [
               SNew(SBox)
               .MinDesiredWidth(MinWidth)
               [
                  SNew(SVerticalBox)
                  + SVerticalBox::Slot()
                  .AutoHeight()
                  [
                     EvasionPropertyHandle->CreatePropertyNameWidget()
                  ]
                  + SVerticalBox::Slot()
                  .AutoHeight()
                  [
                     EvasionPropertyHandle->CreatePropertyValueWidget()
                  ]
               ]
            ]

            // Critical
            +SWrapBox::Slot()
            .Padding(HPadding, 0.f)
            [
               SNew(SBox)
               .MinDesiredWidth(MinWidth)
               [
                  SNew(SVerticalBox)
                  + SVerticalBox::Slot()
                  .AutoHeight()
                  [
                     CriticalPropertyHandle->CreatePropertyNameWidget()
                  ]
                  + SVerticalBox::Slot()
                  .AutoHeight()
                  [
                     CriticalPropertyHandle->CreatePropertyValueWidget()
                  ]
               ]
            ]

            // Move speed
            +SWrapBox::Slot()
            .Padding(HPadding, 0.f)
            [
               SNew(SBox)
               .MinDesiredWidth(MinWidth)
               [
                  SNew(SVerticalBox)
                  + SVerticalBox::Slot()
                  .AutoHeight()
                  [
                     MoveSpeedPropertyHandle->CreatePropertyNameWidget()
                  ]
                  + SVerticalBox::Slot()
                  .AutoHeight()
                  [
                     MoveSpeedPropertyHandle->CreatePropertyValueWidget()
                  ]
               ]
            ]

            // Attack speed
            +SWrapBox::Slot()
            .Padding(HPadding, 0.f)
            [
               SNew(SBox)
               .MinDesiredWidth(MinWidth)
               [
                  SNew(SVerticalBox)
                  + SVerticalBox::Slot()
                  .AutoHeight()
                  [
                     AttackSpeedPropertyHandle->CreatePropertyNameWidget()
                  ]
                  + SVerticalBox::Slot()
                  .AutoHeight()
                  [
                     AttackSpeedPropertyHandle->CreatePropertyValueWidget()
                  ]
               ]
            ]

            // Attack power
            +SWrapBox::Slot()
            .Padding(HPadding, 0.f)
            [
               SNew(SBox)
               .MinDesiredWidth(MinWidth)
               [
                  SNew(SVerticalBox)
                  + SVerticalBox::Slot()
                  .AutoHeight()
                  [
                     AttackPowerPropertyHandle->CreatePropertyNameWidget()
                  ]
                  + SVerticalBox::Slot()
                  .AutoHeight()
                  [
                     AttackPowerPropertyHandle->CreatePropertyValueWidget()
                  ]
               ]
            ]
         ]
      ]
   ];
}

#undef LOCTEXT_NAMESPACE

