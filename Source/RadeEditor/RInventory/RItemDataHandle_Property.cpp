#include "RItemDataHandle_Property.h"

#include "IDetailChildrenBuilder.h"
#include "SlateBasics.h"
#include "DetailLayoutBuilder.h"
#include "DetailWidgetRow.h"
#include "PropertyHandle.h"
#include "EditorStyleSet.h"
#include "PropertyCustomizationHelpers.h"

#define LOCTEXT_NAMESPACE "RadeEditor"

TSharedRef<IPropertyTypeCustomization> FRItemDataHandle_Property::MakeInstance ()
{
	return MakeShareable (new FRItemDataHandle_Property ());
}

void FRItemDataHandle_Property::CustomizeHeader (
	TSharedRef<IPropertyHandle>      StructPropertyHandle,
	FDetailWidgetRow&						HeaderRow,
	IPropertyTypeCustomizationUtils& StructCustomizationUtils)
{
	HeaderRow.NameContent()[StructPropertyHandle->CreatePropertyNameWidget()];
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

	// --- Perform checks
	check (  ArchPropertyHandle.IsValid ()
			&& CountPropertyHandle.IsValid ());

	const float MinWidth = 70.f;
	const float HPadding = 10.f;

	// Draw
	StructBuilder.AddCustomRow(LOCTEXT("FRItemDataHandleRow", "FRItemDataHandle"))
	[
		// ROOT
		SNew(SBorder)
		.BorderImage(FAppStyle::GetBrush("ToolPanel.GroupBorder"))
		.Content()
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
						ArchPropertyHandle->CreatePropertyNameWidget()
					]
					+ SVerticalBox::Slot()
					.AutoHeight()
					[
						ArchPropertyHandle->CreatePropertyValueWidget()
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
						CountPropertyHandle->CreatePropertyNameWidget()
					]
					+ SVerticalBox::Slot()
					.AutoHeight()
					[
						CountPropertyHandle->CreatePropertyValueWidget()
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
						StructBuilder.GenerateStructValueWidget( ArchPropertyHandle.ToSharedRef() )
					]
				]
			]
			+SWrapBox::Slot()
			.Padding(HPadding, 0.f)
			[
				SNew(SObjectPropertyEntryBox)
				.PropertyHandle(ArchPropertyHandle)
			]
		]
	];
}

#undef LOCTEXT_NAMESPACE

