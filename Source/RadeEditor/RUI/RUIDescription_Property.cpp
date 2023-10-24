#include "RUIDescription_Property.h"

#include "IDetailChildrenBuilder.h"
#include "SlateBasics.h"
#include "DetailLayoutBuilder.h"
#include "DetailWidgetRow.h"
#include "PropertyHandle.h"
#include "EditorStyleSet.h"

#define LOCTEXT_NAMESPACE "RadeEditor"

TSharedRef<IPropertyTypeCustomization> FRUIDescription_Property::MakeInstance ()
{
	return MakeShareable (new FRUIDescription_Property ());
}

void FRUIDescription_Property::CustomizeHeader (
	TSharedRef<IPropertyHandle>      StructPropertyHandle,
	FDetailWidgetRow&						HeaderRow,
	IPropertyTypeCustomizationUtils& StructCustomizationUtils)
{
	HeaderRow.NameContent()[StructPropertyHandle->CreatePropertyNameWidget()];
}

void FRUIDescription_Property::CustomizeChildren (
	TSharedRef<IPropertyHandle>      StructPropertyHandle,
	IDetailChildrenBuilder&          StructBuilder,
	IPropertyTypeCustomizationUtils& StructCustomizationUtils)
{
	// --- Get handles
	TSharedPtr<IPropertyHandle> IconPropertyHandle
		= StructPropertyHandle->GetChildHandle(GET_MEMBER_NAME_CHECKED(FRUIDescription, Icon));

	TSharedPtr<IPropertyHandle> LabelPropertyHandle
		= StructPropertyHandle->GetChildHandle(GET_MEMBER_NAME_CHECKED(FRUIDescription, Label));

	TSharedPtr<IPropertyHandle> TooltipPropertyHandle
		= StructPropertyHandle->GetChildHandle(GET_MEMBER_NAME_CHECKED(FRUIDescription, Tooltip));

	// Draw
	StructBuilder.AddCustomRow(LOCTEXT("RUIDescriptionRow", "RUIDescription"))
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
			.Padding(5.f, 0.f)
			[
				// Icon
				SNew(SVerticalBox)
				+ SVerticalBox::Slot()
				.AutoHeight()
				[
					IconPropertyHandle->CreatePropertyValueWidget()
				]
			]
			+SWrapBox::Slot()
			.Padding(5.f, 0.f)
			.FillEmptySpace(true)
			[
				// Label + Tooltip vertically
				SNew(SVerticalBox)
				+ SVerticalBox::Slot()
				.AutoHeight()
				[
					SNew(SVerticalBox)

					// Label
					+ SVerticalBox::Slot()
					[
						SNew(SHorizontalBox)
						+ SHorizontalBox::Slot()
						.Padding(10.f, 5.f, 18.f, 5.f)
						.AutoWidth()
						[
							LabelPropertyHandle->CreatePropertyNameWidget()
						]
						+ SHorizontalBox::Slot()
						.FillWidth(1)
						[
							LabelPropertyHandle->CreatePropertyValueWidget()
						]
					]

					// Tooltip
					+ SVerticalBox::Slot()
					[
						SNew(SHorizontalBox)
						+ SHorizontalBox::Slot()
						.Padding(10.f, 5.f)
						.AutoWidth()
						[
							TooltipPropertyHandle->CreatePropertyNameWidget()
						]
						+ SHorizontalBox::Slot()
						.FillWidth(1)
						[
							TooltipPropertyHandle->CreatePropertyValueWidget()
						]
					]
				]
			]
		]
	];
}

#undef LOCTEXT_NAMESPACE

