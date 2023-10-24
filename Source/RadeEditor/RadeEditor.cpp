#include "RadeEditor.h"

#include "Modules/ModuleInterface.h"
#include "Modules/ModuleManager.h"
#include "PropertyEditorModule.h"

#include "RUI/RUIDescription_Property.h"
#include "RStatus/RCoreStats_Property.h"
#include "RStatus/RSubStats_Property.h"
#include "RStatus/RStatusValue_Property.h"
#include "RStatus/RPassiveStatusEffect_Property.h"
#include "RExperience/REnemyExp_Property.h"
#include "RInventory/RItemDataHandle_Property.h"
#include "RInventory/RItemData_Property.h"

IMPLEMENT_GAME_MODULE(FRadeEditorModule, RadeEditor);

#define LOCTEXT_NAMESPACE "RadeEditor"

void FRadeEditorModule::StartupModule()
{
	FPropertyEditorModule& PropertyModule = FModuleManager::LoadModuleChecked<FPropertyEditorModule> ("PropertyEditor");

	// --- Register Custom property

	// RUIDescription
	PropertyModule.RegisterCustomPropertyTypeLayout (
		FRUIDescription::StaticStruct ()->GetFName (),
		FOnGetPropertyTypeCustomizationInstance::CreateStatic (&FRUIDescription_Property::MakeInstance));

	// RCoreStats
	PropertyModule.RegisterCustomPropertyTypeLayout (
		FRCoreStats::StaticStruct ()->GetFName (),
		FOnGetPropertyTypeCustomizationInstance::CreateStatic (&FRCoreStats_Property::MakeInstance));

	// RSubStats
	PropertyModule.RegisterCustomPropertyTypeLayout (
		FRSubStats::StaticStruct ()->GetFName (),
		FOnGetPropertyTypeCustomizationInstance::CreateStatic (&FRSubStats_Property::MakeInstance));

	// RStatusValue
	PropertyModule.RegisterCustomPropertyTypeLayout (
		FRStatusValue::StaticStruct ()->GetFName (),
		FOnGetPropertyTypeCustomizationInstance::CreateStatic (&FRStatusValue_Property::MakeInstance));

	// RPassiveStatusEffect
	PropertyModule.RegisterCustomPropertyTypeLayout (
		FRPassiveStatusEffect::StaticStruct ()->GetFName (),
		FOnGetPropertyTypeCustomizationInstance::CreateStatic (&FRPassiveStatusEffect_Property::MakeInstance));

	// REnemyExp
	PropertyModule.RegisterCustomPropertyTypeLayout (
		FREnemyExp::StaticStruct ()->GetFName (),
		FOnGetPropertyTypeCustomizationInstance::CreateStatic (&FREnemyExp_Property::MakeInstance));

	/*
	// RItemHandle
	PropertyModule.RegisterCustomPropertyTypeLayout (
		FRItemDataHandle::StaticStruct ()->GetFName (),
		FOnGetPropertyTypeCustomizationInstance::CreateStatic (&FRItemDataHandle_Property::MakeInstance));


	// RItemData
	PropertyModule.RegisterCustomPropertyTypeLayout (
		FRItemData::StaticStruct ()->GetFName (),
		FOnGetPropertyTypeCustomizationInstance::CreateStatic (&FRItemData_Property::MakeInstance));
	*/

	PropertyModule.NotifyCustomizationModuleChanged();
}

void FRadeEditorModule::ShutdownModule()
{
	if (FModuleManager::Get ().IsModuleLoaded ("PropertyEditor")) {
		// unregister properties when the module is shutdown
		FPropertyEditorModule& PropertyModule = FModuleManager::GetModuleChecked<FPropertyEditorModule> ("PropertyEditor");

		PropertyModule.UnregisterCustomPropertyTypeLayout ("RUIDescription");
		PropertyModule.UnregisterCustomPropertyTypeLayout ("RCoreStats");
		PropertyModule.UnregisterCustomPropertyTypeLayout ("RSubStats");
		PropertyModule.UnregisterCustomPropertyTypeLayout ("RStatusValue");
		PropertyModule.UnregisterCustomPropertyTypeLayout ("RPassiveStatusEffect");
		PropertyModule.UnregisterCustomPropertyTypeLayout ("REnemyExp");
		//PropertyModule.UnregisterCustomPropertyTypeLayout ("RItemDataHandle");
		//PropertyModule.UnregisterCustomPropertyTypeLayout ("RItemData");

		PropertyModule.NotifyCustomizationModuleChanged ();
	}
}

#undef LOCTEXT_NAMESPACE
