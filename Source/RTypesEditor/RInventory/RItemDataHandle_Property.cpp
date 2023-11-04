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
   // Empty
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

   const float TableMinWidth = 160.f;
   const float RowMinWidth = 120.f;
   const float HPadding = 5.f;
   bool IsInArray = StructPropertyHandle->GetIndexInArray () != INDEX_NONE;

   TSharedPtr<SWidget> DeleteArrayItemWidget;
   if (IsInArray) {
      const float ButtonSize = 25.f;

	   DeleteArrayItemWidget =
         SNew(SBox)
			.HAlign (HAlign_Center)
			.VAlign (VAlign_Center)
			[
				SNew(SButton)
				.ContentPadding (0)
            .OnClicked_Lambda ([this, StructPropertyHandle] {
               if (!StructPropertyHandle->IsValidHandle ()) return FReply::Unhandled ();
               TSharedPtr<IPropertyHandle> ParentHandle = StructPropertyHandle->GetParentHandle ();
               if (!ParentHandle.IsValid ()) return FReply::Unhandled ();
               TSharedPtr<IPropertyHandleArray> ArrayHandle = ParentHandle->AsArray ();
               if (!ArrayHandle.IsValid ()) return FReply::Unhandled ();

               int32  ChildIndex = StructPropertyHandle->GetArrayIndex ();
               uint32 ChildNum   = 0;
               ArrayHandle->GetNumElements (ChildNum);

               if (ChildIndex < (int32)ChildNum && ChildNum) {
                  ArrayHandle->DeleteItem (ChildIndex);
               }
               return FReply::Handled ();
            })
				[ 
					SNew(SImage)
					.Image (FAppStyle::GetBrush ("Icons.Delete"))
					.ColorAndOpacity (FSlateColor::UseForeground ())
               .DesiredSizeOverride (FVector2D (ButtonSize, ButtonSize))
				]
         ];
   } else {
      DeleteArrayItemWidget = SNullWidget::NullWidget;
   }

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
         SNew(SHorizontalBox)
         +SHorizontalBox::Slot()
         .FillWidth (1)
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

            +SVerticalBox::Slot()
            .AutoHeight()
            [
               SNew(SWrapBox)
               .UseAllottedWidth(true)

               // Table
               +SWrapBox::Slot()
               .Padding (HPadding, 0.f)
               .FillEmptySpace (true)
               [
                  SNew(SBox)
                  .MinDesiredWidth (TableMinWidth)
                  [
                     SNew(SVerticalBox)
                     + SVerticalBox::Slot()
                     .AutoHeight ()
                     .HAlign (EHorizontalAlignment::HAlign_Center)
                     [
                        TableHandle->CreatePropertyNameWidget()
                     ]
                     + SVerticalBox::Slot()
                     .AutoHeight ()
                     [
                        SNew(SObjectPropertyEntryBox)
		                  .AllowedClass (UDataTable::StaticClass ())
                        .OnShouldFilterAsset_Lambda ([this] (const FAssetData& AssetData) {
                           if (UDataTable *ItTable = Cast<UDataTable>(AssetData.GetAsset ())) {
                              if (ItTable->GetRowStruct ()->IsChildOf (FRItemData::StaticStruct ())) return false;
                           }
                           return true;
                        })
		                  .AllowClear (true)
                        .ObjectPath_Lambda ([this, TableHandle] () {
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

               +SWrapBox::Slot ()
               [
                  SNew(SHorizontalBox)
                  +SHorizontalBox::Slot()
                  .FillWidth (1)
                  [
                     SNullWidget::NullWidget
                  ]
               ]

               // Row
               + SWrapBox::Slot ()
               .FillEmptySpace (true)
               [
                  SNew(SBox)
                  .MinDesiredWidth (RowMinWidth)
                  [
                     SNew(SVerticalBox)
                     + SVerticalBox::Slot ()
                     .AutoHeight ()
                     .HAlign (EHorizontalAlignment::HAlign_Center)
                     [
                        RowHandle->CreatePropertyNameWidget ()
                     ]
                     + SVerticalBox::Slot ()
                     .AutoHeight ()
                     [
                        ComboBoxWidget
                     ]
                  ]
               ]

               // Count button
               +SWrapBox::Slot ()
               .Padding (HPadding, 0.f)
               [

                  SNew(SVerticalBox)
                  + SVerticalBox::Slot ()
                  .AutoHeight ()
                  .HAlign (EHorizontalAlignment::HAlign_Center)
                  [
                     CountPropertyHandle->CreatePropertyNameWidget()
                  ]
                  + SVerticalBox::Slot ()
                  .AutoHeight ()
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
                     .Padding (HPadding, 0.f)
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
         + SHorizontalBox::Slot ()
         .AutoWidth ()
         .VAlign (EVerticalAlignment::VAlign_Center)
         [
            DeleteArrayItemWidget.ToSharedRef ()
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

