// Copyright 2015-2023 Vagen Ayrapetyan

#include "RCoreStats_Property.h"
#include "RStatusLib/RStatusTypes.h"

#include "IDetailChildrenBuilder.h"
#include "SlateBasics.h"
#include "DetailLayoutBuilder.h"
#include "DetailWidgetRow.h"
#include "PropertyHandle.h"
#include "EditorStyleSet.h"

#define LOCTEXT_NAMESPACE "RTypesEditor"

TSharedRef<IPropertyTypeCustomization> FRCoreStats_Property::MakeInstance ()
{
   return MakeShareable (new FRCoreStats_Property ());
}

void FRCoreStats_Property::CustomizeHeader (
   TSharedRef<IPropertyHandle>      StructPropertyHandle,
   FDetailWidgetRow&                  HeaderRow,
   IPropertyTypeCustomizationUtils& StructCustomizationUtils)
{
   // Don't show header if not an array item
   if (StructPropertyHandle->GetIndexInArray () != INDEX_NONE) {
      HeaderRow.NameContent ()[StructPropertyHandle->CreatePropertyNameWidget()];
   }
}

void FRCoreStats_Property::CustomizeChildren (
   TSharedRef<IPropertyHandle>      StructPropertyHandle,
   IDetailChildrenBuilder&          StructBuilder,
   IPropertyTypeCustomizationUtils& StructCustomizationUtils)
{
   // --- Get handles
   TSharedPtr<IPropertyHandle> StrPropertyHandle
      = StructPropertyHandle->GetChildHandle(GET_MEMBER_NAME_CHECKED(FRCoreStats, STR));

   TSharedPtr<IPropertyHandle> AgiPropertyHandle
      = StructPropertyHandle->GetChildHandle(GET_MEMBER_NAME_CHECKED(FRCoreStats, AGI));

   TSharedPtr<IPropertyHandle> IntPropertyHandle
      = StructPropertyHandle->GetChildHandle(GET_MEMBER_NAME_CHECKED(FRCoreStats, INT));

   // --- Perform checks
   check (  StrPropertyHandle.IsValid ()
         && AgiPropertyHandle.IsValid ()
         && IntPropertyHandle.IsValid ());

   const float MinWidth = 70.f;
   const float HPadding = 10.f;
   bool IsInArray = StructPropertyHandle->GetIndexInArray () != INDEX_NONE;

   // Draw
   StructBuilder.AddCustomRow(LOCTEXT("FRCoreStatsRow", "FRCoreStats"))
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
                     StrPropertyHandle->CreatePropertyNameWidget()
                  ]
                  + SVerticalBox::Slot()
                  .AutoHeight()
                  [
                     StrPropertyHandle->CreatePropertyValueWidget()
                  ]
               ]
            ]
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
                     AgiPropertyHandle->CreatePropertyNameWidget()
                  ]
                  + SVerticalBox::Slot()
                  .AutoHeight()
                  [
                     AgiPropertyHandle->CreatePropertyValueWidget()
                  ]
               ]
            ]
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
                     IntPropertyHandle->CreatePropertyNameWidget()
                  ]
                  + SVerticalBox::Slot()
                  .AutoHeight()
                  [
                     IntPropertyHandle->CreatePropertyValueWidget()
                  ]
               ]
            ]
         ]
      ]
   ];
}

#undef LOCTEXT_NAMESPACE

