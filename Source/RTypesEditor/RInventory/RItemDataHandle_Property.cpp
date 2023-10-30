// Copyright 2015-2023 Vagen Ayrapetyan

#include "RItemDataHandle_Property.h"
#include "RInventoryLib/RItemTypes.h"

#include "IDetailChildrenBuilder.h"
#include "SlateBasics.h"
#include "DetailLayoutBuilder.h"
#include "PropertyHandle.h"
#include "PropertyCustomizationHelpers.h"

#include "SSearchableComboBox.h"

#define LOCTEXT_NAMESPACE "RTypesEditor"

TSharedRef<IPropertyTypeCustomization> FRItemDataHandle_Property::MakeInstance ()
{
   return MakeShareable (new FRItemDataHandle_Property ());
}

void FRItemDataHandle_Property::CustomizeHeader (
   TSharedRef<IPropertyHandle>      StructPropertyHandle,
   FDetailWidgetRow&                HeaderRow,
   IPropertyTypeCustomizationUtils& StructCustomizationUtils)
{
   // Don't show header if not an array item
   if (StructPropertyHandle->GetIndexInArray () != INDEX_NONE) {
      HeaderRow.NameContent ()[StructPropertyHandle->CreatePropertyNameWidget()];
   }
}

void FRItemDataHandle_Property::CustomizeChildren (
   TSharedRef<IPropertyHandle>      StructPropertyHandle,
   IDetailChildrenBuilder&          StructBuilder,
   IPropertyTypeCustomizationUtils& StructCustomizationUtils)
{

   // --- Get handles
   TSharedPtr<IPropertyHandle> ArchPropertyHandle
      = StructPropertyHandle->GetChildHandle(GET_MEMBER_NAME_CHECKED(FRItemDataHandle, Arch));

   TSharedPtr<IPropertyHandle> CountPropertyHandle
      = StructPropertyHandle->GetChildHandle(GET_MEMBER_NAME_CHECKED(FRItemDataHandle, Count));

   {
      // --- Perform checks
      check (  ArchPropertyHandle.IsValid ()
            && CountPropertyHandle.IsValid ());

      uint32 ChildCount = 0;
      ArchPropertyHandle->GetNumChildren (ChildCount);
      check (ChildCount == 2);
   }

   const TSharedRef<IPropertyHandle> TableHandle = ArchPropertyHandle->GetChildHandle (0).ToSharedRef();
   const TSharedRef<IPropertyHandle> RowHandle   = ArchPropertyHandle->GetChildHandle (1).ToSharedRef();

   const float MinWidth = 160.f;
   const float HPadding = 10.f;
   bool IsInArray = StructPropertyHandle->GetIndexInArray () != INDEX_NONE;

   // Create initial list
   TSharedRef<SSearchableComboBox> ComboBoxWidget = SNew(SSearchableComboBox)
      .OptionsSource(&RowNames)
      .OnSelectionChanged_Lambda ([this, RowHandle] (TSharedPtr<FString> ProposedSelection, ESelectInfo::Type SelectInfo) {
         FName Result = FName (*ProposedSelection);
         if (RowHandle->IsValidHandle ()) RowHandle->SetValue (Result);
      })
      .OnGenerateWidget_Lambda ([this] (TSharedPtr<FString> InItem) {
	      return SNew(STextBlock)
		      .Text(FText::FromString(InItem.IsValid() ? *InItem : FString()))
		      .Font(IDetailLayoutBuilder::GetDetailFont());
      })
      .Content()
      [
         SNew(STextBlock)
         .Text(MakeAttributeLambda ([this, RowHandle] () {
            FName Result;
            if (RowHandle->IsValidHandle ()) RowHandle->GetValue (Result);
            return FText::FromName (Result);
         }))
      ];

   // Get initial value
   {
      FAssetData AssetData;
      TableHandle->GetValue (AssetData);
      RefreshRowList (ComboBoxWidget, Cast<UDataTable>(AssetData.GetAsset ()));
   }

   // Draw
   StructBuilder.AddCustomRow(LOCTEXT("FRItemDataHandleRow", "FRItemDataHandle"))
   [
      // ROOT
      SNew(SBorder)
      .BorderImage(FAppStyle::GetBrush("ToolPanel.GroupBorder"))
      .Content()
      [
         SNew(SVerticalBox)

         // Header
         +SVerticalBox::Slot()
         .AutoHeight()
         .HAlign (EHorizontalAlignment::HAlign_Center)
         [
            SNew(STextBlock)
            .Visibility_Lambda ([this, IsInArray] { return IsInArray ? EVisibility::Collapsed : EVisibility::Visible; })
            .Text (StructPropertyHandle->GetPropertyDisplayName ())
         ]

         // Table
         +SVerticalBox::Slot()
         .AutoHeight()
         [
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
                  .HAlign (EHorizontalAlignment::HAlign_Center)
                  [
                     TableHandle->CreatePropertyNameWidget()
                  ]
                  + SVerticalBox::Slot()
                  .AutoHeight()
                  [
                     SNew(SObjectPropertyEntryBox)
		               .AllowedClass(UDataTable::StaticClass())
                     .OnShouldFilterAsset_Lambda ([this] (const FAssetData& AssetData) {
                        if (UDataTable *ItTable = Cast<UDataTable>(AssetData.GetAsset ())) {
                           if (ItTable->GetRowStruct ()->IsChildOf (FRItemData::StaticStruct ())) return false;
                        }
                        return true;
                     })
		               .AllowClear(true)
                     .ObjectPath_Lambda([this, TableHandle] () {
                        FString Result; 
                        if (TableHandle->IsValidHandle ()) TableHandle->GetValueAsDisplayString (Result);
                        return Result;
                     })
		               .OnObjectChanged_Lambda ([this, TableHandle, RowHandle, ComboBoxWidget] (const FAssetData& AssetData) {
                        if (!TableHandle->IsValidHandle ()) return;
                        if (!RowHandle->IsValidHandle ()) return;

                        TableHandle->SetValue (AssetData);
                        RowHandle->SetValue (NAME_None);
                        RefreshRowList (ComboBoxWidget, Cast<UDataTable>(AssetData.GetAsset ()));
                     })
                  ]
            ]
            ]

            // Row
            + SWrapBox::Slot()
            .Padding(HPadding, 0.f)
            [
               SNew(SBox)
               .MinDesiredWidth(MinWidth)
               [
                  SNew(SVerticalBox)
                  + SVerticalBox::Slot()
                  .AutoHeight()
                  .HAlign (EHorizontalAlignment::HAlign_Center)
                  [
                     RowHandle->CreatePropertyNameWidget()
                  ]
                  + SVerticalBox::Slot()
                  .AutoHeight()
                  [
                     ComboBoxWidget
                  ]
               ]
            ]

            // Count button
            +SWrapBox::Slot()
            .Padding(HPadding * 2, 0.f)
            [

               SNew(SVerticalBox)
               + SVerticalBox::Slot()
               .AutoHeight()
               .HAlign (EHorizontalAlignment::HAlign_Center)
               [
                  CountPropertyHandle->CreatePropertyNameWidget()
               ]
               + SVerticalBox::Slot()
               .AutoHeight()
               .VAlign (EVerticalAlignment::VAlign_Center)
               [
                  SNew(SHorizontalBox)
                  +SHorizontalBox::Slot ()
                  .AutoWidth ()
                  [
                     SNew(SButton)
                     .OnClicked_Lambda ([this, CountPropertyHandle] {
                        if (!CountPropertyHandle->IsValidHandle ()) return FReply::Unhandled ();
                        int32 Current = 0;
                        CountPropertyHandle->GetValue (Current);
                        CountPropertyHandle->SetValue (Current - 1);
                        return FReply::Handled ();
                     })
                     [
                           SNew(STextBlock)
                        .Text (LOCTEXT("CountDecreaseBtn", "-"))
                     ]
                  ]
                  +SHorizontalBox::Slot ()
                  .AutoWidth ()
                  .Padding(HPadding, 0.f)
                  [
                     CountPropertyHandle->CreatePropertyValueWidget()
                  ]
                  +SHorizontalBox::Slot ()
                  .AutoWidth ()
                  [
                     SNew(SButton)
                     .OnClicked_Lambda ([this, CountPropertyHandle] {
                        if (!CountPropertyHandle->IsValidHandle ()) return FReply::Unhandled ();
                        int32 Current = 0;
                        CountPropertyHandle->GetValue (Current);
                        CountPropertyHandle->SetValue (Current + 1);
                        return FReply::Handled ();
                     })
                     [
                           SNew(STextBlock)
                        .Text (LOCTEXT("CountIncreaseBtn", "+"))
                     ]
                  ]
               ]
            ]
         ]
      ]
   ];

   // Default implementation
   //StructBuilder.AddProperty (ArchPropertyHandle.ToSharedRef ());
   //StructBuilder.AddProperty (CountPropertyHandle.ToSharedRef ());
}

void FRItemDataHandle_Property::RefreshRowList (TSharedRef<SSearchableComboBox> ComboBoxWidget, const UDataTable *DataTable)
{
   RowNames.Empty ();
   if (DataTable) {
      const TArray<FName> &TableRows = DataTable->GetRowNames ();
      for (const FName &ItRow : TableRows) {
         RowNames.Add (MakeShareable (new FString (ItRow.ToString ())));
      } 
   }
   ComboBoxWidget->RefreshOptions ();
}


#undef LOCTEXT_NAMESPACE

