// Copyright 2015-2023 Vagen Ayrapetyan

#include "RItemData_Property.h"
#include "RInventoryLib/RItemTypes.h"

#include "IDetailChildrenBuilder.h"
#include "SlateBasics.h"
#include "DetailLayoutBuilder.h"
#include "DetailWidgetRow.h"
#include "PropertyHandle.h"
#include "EditorStyleSet.h"
#include "../RUI/RUIDescription_Property.h"


#define LOCTEXT_NAMESPACE "RTypesEditor"

TSharedRef<IPropertyTypeCustomization> FRItemData_Property::MakeInstance ()
{
   return MakeShareable (new FRItemData_Property ());
}

void FRItemData_Property::CustomizeHeader (
   TSharedRef<IPropertyHandle>      StructPropertyHandle,
   FDetailWidgetRow&                  HeaderRow,
   IPropertyTypeCustomizationUtils& StructCustomizationUtils)
{
   // Don't show header if not an array item
   if (StructPropertyHandle->GetIndexInArray () != INDEX_NONE) {
      HeaderRow.NameContent ()[StructPropertyHandle->CreatePropertyNameWidget()];
   }
}

void FRItemData_Property::CustomizeChildren (
   TSharedRef<IPropertyHandle>      StructPropertyHandle,
   IDetailChildrenBuilder&          StructBuilder,
   IPropertyTypeCustomizationUtils& StructCustomizationUtils)
{
   // --- Get handles


   // Data table row Unique ID
   TSharedPtr<IPropertyHandle> IDPropertyHandle
      = StructPropertyHandle->GetChildHandle(GET_MEMBER_NAME_CHECKED(FRItemData, ID));

   // Must be defined in all subclasses for type checking and casting
   TSharedPtr<IPropertyHandle> TypePropertyHandle
      = StructPropertyHandle->GetChildHandle(GET_MEMBER_NAME_CHECKED(FRItemData, Type));

   // Into what types can the 'JsonData' be converted to
   TSharedPtr<IPropertyHandle> CastTypePropertyHandle
      = StructPropertyHandle->GetChildHandle(GET_MEMBER_NAME_CHECKED(FRItemData, CastType));

   // UI Information
   TSharedPtr<IPropertyHandle> DescriptionPropertyHandle
      = StructPropertyHandle->GetChildHandle(GET_MEMBER_NAME_CHECKED(FRItemData, Description));

   // Rarity
   TSharedPtr<IPropertyHandle> RarityPropertyHandle
      = StructPropertyHandle->GetChildHandle(GET_MEMBER_NAME_CHECKED(FRItemData, Rarity));

   // Number of item instances
   TSharedPtr<IPropertyHandle> CountPropertyHandle
      = StructPropertyHandle->GetChildHandle(GET_MEMBER_NAME_CHECKED(FRItemData, Count));

   // Max number of item instances per item slot
   // Stackable: MaxCount > 1
   TSharedPtr<IPropertyHandle> MaxCountPropertyHandle
      = StructPropertyHandle->GetChildHandle(GET_MEMBER_NAME_CHECKED(FRItemData, MaxCount));

   // Item Weight of each instance. In gramms.
   TSharedPtr<IPropertyHandle> WeightPropertyHandle
      = StructPropertyHandle->GetChildHandle(GET_MEMBER_NAME_CHECKED(FRItemData, Weight));

   // Selling price
   // 0 -> Can't sell
   TSharedPtr<IPropertyHandle> PricePropertyHandle
      = StructPropertyHandle->GetChildHandle(GET_MEMBER_NAME_CHECKED(FRItemData, Price));

   // --- Pickup
   // Pickup mesh. Will search and update statis mesh component
   TSharedPtr<IPropertyHandle> PickupMeshPropertyHandle
      = StructPropertyHandle->GetChildHandle(GET_MEMBER_NAME_CHECKED(FRItemData, PickupMesh));

   // Custom pickup actor class. Will be used if set.
   TSharedPtr<IPropertyHandle> PickupPropertyHandle
      = StructPropertyHandle->GetChildHandle(GET_MEMBER_NAME_CHECKED(FRItemData, Pickup));

   // --- Perform checks
   check (  IDPropertyHandle.IsValid ()
         && TypePropertyHandle.IsValid ()
         && CastTypePropertyHandle.IsValid ()
         && DescriptionPropertyHandle.IsValid ()
         && RarityPropertyHandle.IsValid ()
         && CountPropertyHandle.IsValid ()
         && MaxCountPropertyHandle.IsValid ()
         && WeightPropertyHandle.IsValid ()
         && PricePropertyHandle.IsValid ()
         && PickupMeshPropertyHandle.IsValid ()
         && PickupPropertyHandle.IsValid ()
   );


   const float MinWidth = 70.f;
   const float HPadding = 10.f;
   bool IsInArray = StructPropertyHandle->GetIndexInArray () != INDEX_NONE;

   StructBuilder.GenerateStructValueWidget(DescriptionPropertyHandle.ToSharedRef());

   // Draw
   StructBuilder.AddCustomRow(LOCTEXT("FRItemDataRow", "FRItemData"))
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
                     RarityPropertyHandle->CreatePropertyNameWidget()
                  ]
                  + SVerticalBox::Slot()
                  .AutoHeight()
                  [
                     RarityPropertyHandle->CreatePropertyValueWidget()
                  ]
               ]
            ]
            +SWrapBox::Slot()
            .Padding(HPadding, 0.f)
            [
               SNew( SBox )
               [
                  StructBuilder.GenerateStructValueWidget( DescriptionPropertyHandle.ToSharedRef() )
               ]
            ]

            +SWrapBox::Slot()
            .Padding(HPadding, 0.f)
            [
               SNew( SBox )
               [
                  StructBuilder.GenerateStructValueWidget( DescriptionPropertyHandle.ToSharedRef() )
               ]
            ]
         ]
      ]
   ];
}

#undef LOCTEXT_NAMESPACE

