#include "REnemyExp_Property.h"

#include "IDetailChildrenBuilder.h"
#include "SlateBasics.h"
#include "DetailLayoutBuilder.h"
#include "DetailWidgetRow.h"
#include "PropertyHandle.h"
#include "EditorStyleSet.h"

#define LOCTEXT_NAMESPACE "RTypesEditor"

TSharedRef<IPropertyTypeCustomization> FREnemyExp_Property::MakeInstance ()
{
	return MakeShareable (new FREnemyExp_Property ());
}

void FREnemyExp_Property::CustomizeHeader (
	TSharedRef<IPropertyHandle>      StructPropertyHandle,
	FDetailWidgetRow&						HeaderRow,
	IPropertyTypeCustomizationUtils& StructCustomizationUtils)
{
	HeaderRow.NameContent()[StructPropertyHandle->CreatePropertyNameWidget()];
}

void FREnemyExp_Property::CustomizeChildren (
	TSharedRef<IPropertyHandle>      StructPropertyHandle,
	IDetailChildrenBuilder&          StructBuilder,
	IPropertyTypeCustomizationUtils& StructCustomizationUtils)
{
	// --- Get handles
	TSharedPtr<IPropertyHandle> TargetPropertyHandle
		= StructPropertyHandle->GetChildHandle(GET_MEMBER_NAME_CHECKED(FREnemyExp, TargetClass));

	TSharedPtr<IPropertyHandle> DamagePropertyHandle
		= StructPropertyHandle->GetChildHandle(GET_MEMBER_NAME_CHECKED(FREnemyExp, PerDamage));

	TSharedPtr<IPropertyHandle> DeathPropertyHandle
		= StructPropertyHandle->GetChildHandle(GET_MEMBER_NAME_CHECKED(FREnemyExp, PerDeath));

	// --- Perform checks
	check (  TargetPropertyHandle.IsValid ()
			&& DamagePropertyHandle.IsValid ()
			&& DeathPropertyHandle.IsValid ());

	const float TargetMinWidth = 260.f;
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
						DamagePropertyHandle->CreatePropertyNameWidget()
					]
					+ SVerticalBox::Slot()
					.AutoHeight()
					[
						DamagePropertyHandle->CreatePropertyValueWidget()
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
						DeathPropertyHandle->CreatePropertyNameWidget()
					]
					+ SVerticalBox::Slot()
					.AutoHeight()
					[
						DeathPropertyHandle->CreatePropertyValueWidget()
					]
				]
			]
		]
	];
}

#undef LOCTEXT_NAMESPACE

