// Copyright 2015-2023 Vagen Ayrapetyan

#pragma once

#include "DetailCustomizations.h"
#include "IPropertyTypeCustomization.h"

class FDetailWidgetRow;
class IDetailChildrenBuilder;
class SSearchableComboBox;
class UDataTable;

class FRItemDataHandle_Property : public IPropertyTypeCustomization
{
public:
   static TSharedRef<IPropertyTypeCustomization> MakeInstance();

   // Header
   virtual void CustomizeHeader (
      TSharedRef<IPropertyHandle>      StructPropertyHandle,
      FDetailWidgetRow&                  HeaderRow,
      IPropertyTypeCustomizationUtils& StructCustomizationUtils) override;

   // Content
   virtual void CustomizeChildren (
      TSharedRef<IPropertyHandle>      StructPropertyHandle,
      IDetailChildrenBuilder&            StructBuilder,
      IPropertyTypeCustomizationUtils& StructCustomizationUtils) override;

private:

   UPROPERTY()
      TArray<TSharedPtr<FString>> RowNames;

   UFUNCTION()
      void RefreshRowList (TSharedRef<SSearchableComboBox> ComboBoxWidget, const UDataTable* DataTable);
};

