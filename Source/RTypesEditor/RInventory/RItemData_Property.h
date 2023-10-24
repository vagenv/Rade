#pragma once


#include "DetailCustomizations.h"
#include "IPropertyTypeCustomization.h"
#include "RInventoryLib/RItemTypes.h"

class FDetailWidgetRow;
class IDetailChildrenBuilder;

class FRItemData_Property : public IPropertyTypeCustomization
{
public:
	static TSharedRef<IPropertyTypeCustomization> MakeInstance();

	// Header
	virtual void CustomizeHeader (
		TSharedRef<IPropertyHandle>		StructPropertyHandle,
		FDetailWidgetRow&						HeaderRow,
		IPropertyTypeCustomizationUtils& StructCustomizationUtils) override;

	// Content
	virtual void CustomizeChildren (
		TSharedRef<IPropertyHandle>		StructPropertyHandle,
		IDetailChildrenBuilder&				StructBuilder,
		IPropertyTypeCustomizationUtils& StructCustomizationUtils) override;
};

