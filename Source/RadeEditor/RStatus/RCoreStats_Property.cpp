#include "RCoreStats_Property.h"

#include "IDetailChildrenBuilder.h"
#include "SlateBasics.h"
#include "DetailLayoutBuilder.h"
#include "DetailWidgetRow.h"
#include "PropertyHandle.h"
#include "EditorStyleSet.h"

#define LOCTEXT_NAMESPACE "RadeEditor"

TSharedRef<IPropertyTypeCustomization> FRCoreStats_Property::MakeInstance ()
{
	return MakeShareable (new FRCoreStats_Property ());
}

void FRCoreStats_Property::CustomizeHeader (
	TSharedRef<IPropertyHandle>      StructPropertyHandle,
	FDetailWidgetRow&						HeaderRow,
	IPropertyTypeCustomizationUtils& StructCustomizationUtils)
{
	HeaderRow.NameContent()[StructPropertyHandle->CreatePropertyNameWidget()];
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

	// Draw
	StructBuilder.AddCustomRow(LOCTEXT("FRCoreStatsRow", "FRCoreStats"))
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
			.Padding(10.f, 0.f)
			[
				SNew(SBox)
				.MinDesiredWidth(70.f)
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
			.Padding(10.f, 0.f)
			[
				SNew(SBox)
				.MinDesiredWidth(70.f)
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
			.Padding(10.f, 0.f)
			[
				SNew(SBox)
				.MinDesiredWidth(70.f)
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
	];
}

#undef LOCTEXT_NAMESPACE

