// Copyright 2015-2023 Vagen Ayrapetyan

#include "RStatusValue_Property.h"
#include "RStatusLib/RStatusTypes.h"

#include "IDetailChildrenBuilder.h"
#include "SlateBasics.h"
#include "DetailLayoutBuilder.h"
#include "DetailWidgetRow.h"
#include "PropertyHandle.h"
#include "EditorStyleSet.h"

#define LOCTEXT_NAMESPACE "RTypesEditor"

TSharedRef<IPropertyTypeCustomization> FRStatusValue_Property::MakeInstance ()
{
	return MakeShareable (new FRStatusValue_Property ());
}

void FRStatusValue_Property::CustomizeHeader (
	TSharedRef<IPropertyHandle>      StructPropertyHandle,
	FDetailWidgetRow&						HeaderRow,
	IPropertyTypeCustomizationUtils& StructCustomizationUtils)
{
	HeaderRow.NameContent()[StructPropertyHandle->CreatePropertyNameWidget()];
}

void FRStatusValue_Property::CustomizeChildren (
	TSharedRef<IPropertyHandle>      StructPropertyHandle,
	IDetailChildrenBuilder&          StructBuilder,
	IPropertyTypeCustomizationUtils& StructCustomizationUtils)
{
	// --- Get handles
	TSharedPtr<IPropertyHandle> CurrentPropertyHandle
		= StructPropertyHandle->GetChildHandle(GET_MEMBER_NAME_CHECKED(FRStatusValue, Current));

	TSharedPtr<IPropertyHandle> MaxPropertyHandle
		= StructPropertyHandle->GetChildHandle(GET_MEMBER_NAME_CHECKED(FRStatusValue, Max));

	TSharedPtr<IPropertyHandle> RegenPropertyHandle
		= StructPropertyHandle->GetChildHandle(GET_MEMBER_NAME_CHECKED(FRStatusValue, Regen));

	// --- Perform checks
	check (  CurrentPropertyHandle.IsValid ()
			&& MaxPropertyHandle.IsValid ()
			&& RegenPropertyHandle.IsValid ());

	const float MinWidth = 70.f;
	const float HPadding = 10.f;

	// Draw
	StructBuilder.AddCustomRow(LOCTEXT("FRStatusValueRow", "FRStatusValue"))
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
						CurrentPropertyHandle->CreatePropertyNameWidget()
					]
					+ SVerticalBox::Slot()
					.AutoHeight()
					[
						CurrentPropertyHandle->CreatePropertyValueWidget()
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
						MaxPropertyHandle->CreatePropertyNameWidget()
					]
					+ SVerticalBox::Slot()
					.AutoHeight()
					[
						MaxPropertyHandle->CreatePropertyValueWidget()
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
						RegenPropertyHandle->CreatePropertyNameWidget()
					]
					+ SVerticalBox::Slot()
					.AutoHeight()
					[
						RegenPropertyHandle->CreatePropertyValueWidget()
					]
				]
			]
		]
	];
}

#undef LOCTEXT_NAMESPACE

