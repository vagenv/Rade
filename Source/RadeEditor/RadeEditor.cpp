#include "RadeEditor.h"

#include "Modules/ModuleInterface.h"
#include "Modules/ModuleManager.h"
#include "PropertyEditorModule.h"

#include "RUI/RUIDescription_Property.h"
#include "RStatus/RCoreStats_Property.h"


IMPLEMENT_GAME_MODULE(FRadeEditorModule, RadeEditor);

#define LOCTEXT_NAMESPACE "RadeEditor"

void FRadeEditorModule::StartupModule()
{
	FPropertyEditorModule& PropertyModule = FModuleManager::LoadModuleChecked<FPropertyEditorModule> ("PropertyEditor");

	// Register Custom property
	PropertyModule.RegisterCustomPropertyTypeLayout (
		FRUIDescription::StaticStruct ()->GetFName (),
		FOnGetPropertyTypeCustomizationInstance::CreateStatic (&FRUIDescription_Property::MakeInstance));

	PropertyModule.RegisterCustomPropertyTypeLayout (
		FRCoreStats::StaticStruct ()->GetFName (),
		FOnGetPropertyTypeCustomizationInstance::CreateStatic (&FRCoreStats_Property::MakeInstance));

	PropertyModule.NotifyCustomizationModuleChanged();
}

void FRadeEditorModule::ShutdownModule()
{
	if (FModuleManager::Get ().IsModuleLoaded ("PropertyEditor")) {
		// unregister properties when the module is shutdown
		FPropertyEditorModule& PropertyModule = FModuleManager::GetModuleChecked<FPropertyEditorModule> ("PropertyEditor");

		PropertyModule.UnregisterCustomPropertyTypeLayout ("RUIDescription");
		PropertyModule.UnregisterCustomPropertyTypeLayout ("RCoreStats");

		PropertyModule.NotifyCustomizationModuleChanged ();
	}
}

#undef LOCTEXT_NAMESPACE
