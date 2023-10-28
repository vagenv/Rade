// Copyright 2015-2023 Vagen Ayrapetyan

#include "RPassiveStatusEffect_Property.h"
#include "RStatusLib/RPassiveStatusEffect.h"

#include "IDetailChildrenBuilder.h"
#include "SlateBasics.h"
#include "DetailLayoutBuilder.h"
#include "DetailWidgetRow.h"
#include "PropertyHandle.h"
#include "EditorStyleSet.h"

#define LOCTEXT_NAMESPACE "RTypesEditor"

TSharedRef<IPropertyTypeCustomization> FRPassiveStatusEffect_Property::MakeInstance ()
{
	return MakeShareable (new FRPassiveStatusEffect_Property ());
}

void FRPassiveStatusEffect_Property::CustomizeHeader (
	TSharedRef<IPropertyHandle>      StructPropertyHandle,
	FDetailWidgetRow&						HeaderRow,
	IPropertyTypeCustomizationUtils& StructCustomizationUtils)
{
	HeaderRow.NameContent()[StructPropertyHandle->CreatePropertyNameWidget()];
}

void FRPassiveStatusEffect_Property::CustomizeChildren (
	TSharedRef<IPropertyHandle>      StructPropertyHandle,
	IDetailChildrenBuilder&          StructBuilder,
	IPropertyTypeCustomizationUtils& StructCustomizationUtils)
{
	// --- Get handles
	TSharedPtr<IPropertyHandle> TargetPropertyHandle
		= StructPropertyHandle->GetChildHandle(GET_MEMBER_NAME_CHECKED(FRPassiveStatusEffect, EffectTarget));

	TSharedPtr<IPropertyHandle> FlatPropertyHandle
		= StructPropertyHandle->GetChildHandle(GET_MEMBER_NAME_CHECKED(FRPassiveStatusEffect, Flat));

	TSharedPtr<IPropertyHandle> PercentPropertyHandle
		= StructPropertyHandle->GetChildHandle(GET_MEMBER_NAME_CHECKED(FRPassiveStatusEffect, Percent));

	// --- Perform checks
	check (  TargetPropertyHandle.IsValid ()
			&& FlatPropertyHandle.IsValid ()
			&& PercentPropertyHandle.IsValid ());

	const float TargetMinWidth = 130.f;
	const float ValueMinWidth  = 70.f;
	const float HPadding       = 10.f;

	// Draw
	StructBuilder.AddCustomRow(LOCTEXT("FRPassiveStatusEffectRow", "FRPassiveStatusEffect"))
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
				.MinDesiredWidth(TargetMinWidth)
				[
					SNew(SVerticalBox)
					+ SVerticalBox::Slot()
					.AutoHeight()
					[
						TargetPropertyHandle->CreatePropertyNameWidget()
					]
					+ SVerticalBox::Slot()
					.AutoHeight()
					[
						TargetPropertyHandle->CreatePropertyValueWidget()
					]
				]
			]
			+SWrapBox::Slot()
			.Padding(HPadding, 0.f)
			[
				SNew(SBox)
				.MinDesiredWidth(ValueMinWidth)
				[
					SNew(SVerticalBox)
					+ SVerticalBox::Slot()
					.AutoHeight()
					[
						FlatPropertyHandle->CreatePropertyNameWidget()
					]
					+ SVerticalBox::Slot()
					.AutoHeight()
					[
						FlatPropertyHandle->CreatePropertyValueWidget()
					]
				]
			]
			+SWrapBox::Slot()
			.Padding(HPadding, 0.f)
			[
				SNew(SBox)
				.MinDesiredWidth(ValueMinWidth)
				[
					SNew(SVerticalBox)
					+ SVerticalBox::Slot()
					.AutoHeight()
					[
						PercentPropertyHandle->CreatePropertyNameWidget()
					]
					+ SVerticalBox::Slot()
					.AutoHeight()
					[
						PercentPropertyHandle->CreatePropertyValueWidget()
					]
				]
			]
		]
	];
}

#undef LOCTEXT_NAMESPACE

