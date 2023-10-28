// Copyright 2015-2023 Vagen Ayrapetyan

#include "RTypesEditor.h"

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

IMPLEMENT_GAME_MODULE(RTypesEditorModule, RTypesEditor);

#define LOCTEXT_NAMESPACE "RTypesEditor"

void RTypesEditorModule::StartupModule()
{
   FPropertyEditorModule& PropertyModule = FModuleManager::LoadModuleChecked<FPropertyEditorModule> ("PropertyEditor");

   // --- Register Custom property

   // RUIDescription
   PropertyModule.RegisterCustomPropertyTypeLayout (
      "RUIDescription",
      FOnGetPropertyTypeCustomizationInstance::CreateStatic (&FRUIDescription_Property::MakeInstance));

   // RCoreStats
   PropertyModule.RegisterCustomPropertyTypeLayout (
      "RCoreStats",
      FOnGetPropertyTypeCustomizationInstance::CreateStatic (&FRCoreStats_Property::MakeInstance));

   // RSubStats
   PropertyModule.RegisterCustomPropertyTypeLayout (
      "RSubStats",
      FOnGetPropertyTypeCustomizationInstance::CreateStatic (&FRSubStats_Property::MakeInstance));

   // RStatusValue
   PropertyModule.RegisterCustomPropertyTypeLayout (
      "RStatusValue",
      FOnGetPropertyTypeCustomizationInstance::CreateStatic (&FRStatusValue_Property::MakeInstance));

   // RPassiveStatusEffect
   PropertyModule.RegisterCustomPropertyTypeLayout (
      "RPassiveStatusEffect",
      FOnGetPropertyTypeCustomizationInstance::CreateStatic (&FRPassiveStatusEffect_Property::MakeInstance));

   // REnemyExp
   PropertyModule.RegisterCustomPropertyTypeLayout (
      "REnemyExp",
      FOnGetPropertyTypeCustomizationInstance::CreateStatic (&FREnemyExp_Property::MakeInstance));


   /*
   // RItemHandle
   PropertyModule.RegisterCustomPropertyTypeLayout (
      "RItemDataHandle",
      FOnGetPropertyTypeCustomizationInstance::CreateStatic (&FRItemDataHandle_Property::MakeInstance));

   // RItemData
   PropertyModule.RegisterCustomPropertyTypeLayout (
      "RItemData",
      FOnGetPropertyTypeCustomizationInstance::CreateStatic (&FRItemData_Property::MakeInstance));
   */

   PropertyModule.NotifyCustomizationModuleChanged ();
}

void RTypesEditorModule::ShutdownModule()
{
   if (FModuleManager::Get ().IsModuleLoaded ("PropertyEditor")) {

      // --- Unregister properties when the module is shutdown
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
