// Copyright 2015-2023 Vagen Ayrapetyan

#include "ROptionManager.h"
#include "AudioDevice.h"
#include "InputCore.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "GameFramework/InputSettings.h"
#include "GameFramework/GameUserSettings.h"
#include "Math/UnrealMathUtility.h"
#include "RUtilLib/Rlog.h"


// Get Game User Setting
UGameUserSettings* UROptionManager::GetGameUserSettings ()
{
   return (GEngine != nullptr ? GEngine->GameUserSettings : nullptr);
}

bool FRScreenResolution::operator == (const FRScreenResolution &res) const noexcept
{
   return (Width == res.Width && Height == res.Height);
}

bool FRVideoQualitySetting::operator == (const FRVideoQualitySetting &res) const noexcept
{
   return (
      VSyncEnabled       == res.VSyncEnabled       &&
      FrameRate          == res.FrameRate          &&
      ResolutionQuality  == res.ResolutionQuality  &&
      AntiAliasing       == res.AntiAliasing       &&
      DynamicResolution  == res.DynamicResolution  &&
      ViewDistance       == res.ViewDistance       &&
      TextureQuality     == res.TextureQuality     &&
      ShadowQuality      == res.ShadowQuality      &&
      ShadingQuality     == res.ShadingQuality     &&
      GlobalIllumination == res.GlobalIllumination &&
      ReflectionQuality  == res.ReflectionQuality  &&
      FoliageQuality     == res.FoliageQuality     &&
      EffectQuality      == res.EffectQuality      &&
      PostProcessQuality == res.PostProcessQuality
   );
}


FString UROptionUtilFunc::FRScreenResolution_ToString (const FRScreenResolution &Resolution)
{
   return FString::Printf (TEXT("%lldx%lld"), Resolution.Width, Resolution.Height);
}

bool UROptionUtilFunc::FRScreenResolution_EqualEqual (const FRScreenResolution& A, const FRScreenResolution& B)
{
   return A == B;
}

FRScreenResolution UROptionUtilFunc::FRScreenResolution_FromString (const FString &Resolution)
{
   FRScreenResolution res;
   FString Width;
   FString Height;
   if (Resolution.Split (FString ("x"), &Width, &Height)) {
      res.Width  = FCString::Atoi (*Width);
      res.Height = FCString::Atoi (*Height);
   }
   return res;
}

bool UROptionUtilFunc::FRVideoQualitySetting_EqualEqual (const FRVideoQualitySetting& A, const FRVideoQualitySetting& B)
{
   return A == B;
}

//==========================================================================//
//                Screen resolution
//==========================================================================//

// Get List of Supported Resolutions
bool UROptionManager::GetSupportedScreenResolutions (TArray<FRScreenResolution>& Resolutions)
{
   FScreenResolutionArray ResolutionsArray;
   if (RHIGetAvailableResolutions (ResolutionsArray, true)){
      for (const FScreenResolutionRHI& Resolution : ResolutionsArray){
         FRScreenResolution res;
         res.Height = Resolution.Height;
         res.Width  = Resolution.Width;
         Resolutions.AddUnique (res);
      }
      return true;
   }
   return false;
}

bool UROptionManager::GetCurrentScreenResolution (FRScreenResolution &Resolutions, TEnumAsByte<EWindowMode::Type> &WindowMode)
{
   UGameUserSettings* Settings = GetGameUserSettings ();
   if (!ensure (Settings)) return false;

   // Resolution
   FIntPoint res = Settings->GetScreenResolution ();
   Resolutions.Width = res.X;
   Resolutions.Height = res.Y;

   // Window Mode
   WindowMode = Settings->GetFullscreenMode ();

   return true;
}

// Change the current screen resolution
bool UROptionManager::SetScreenResolution (const FRScreenResolution &Resolution, const EWindowMode::Type WindowMode)
{
   UGameUserSettings* Settings = GetGameUserSettings ();
   if (!ensure (Settings)) return false;

   bool checkCmdLine = false;
   Settings->SetScreenResolution (FIntPoint(Resolution.Width, Resolution.Height));
   Settings->SetFullscreenMode (WindowMode);

   Settings->ApplyResolutionSettings (checkCmdLine);
   Settings->SaveSettings ();
   return true;
}

//==========================================================================//
//                Video Quality Settings
//==========================================================================//

// Get current video quality
bool UROptionManager::GetCurrentVideoQualitySettings (FRVideoQualitySetting& QualitySettings)
{
   UGameUserSettings* Settings = GetGameUserSettings ();
   if (!ensure (Settings)) return false;

   QualitySettings.VSyncEnabled       = Settings->IsVSyncEnabled ();
   QualitySettings.FrameRate          = Settings->GetFrameRateLimit ();
   QualitySettings.ResolutionQuality  = Settings->GetResolutionScaleNormalized ();
   QualitySettings.DynamicResolution  = Settings->IsDynamicResolutionEnabled ();
   QualitySettings.AntiAliasing       = Settings->GetAntiAliasingQuality ();
   QualitySettings.ViewDistance       = Settings->GetViewDistanceQuality ();
   QualitySettings.TextureQuality     = Settings->GetTextureQuality ();
   QualitySettings.ShadowQuality      = Settings->GetShadowQuality ();
   QualitySettings.ShadingQuality     = Settings->GetShadingQuality ();
   QualitySettings.GlobalIllumination = Settings->GetGlobalIlluminationQuality ();
   QualitySettings.ReflectionQuality  = Settings->GetReflectionQuality ();
   QualitySettings.FoliageQuality     = Settings->GetFoliageQuality ();
   QualitySettings.EffectQuality      = Settings->GetVisualEffectQuality ();
   QualitySettings.PostProcessQuality = Settings->GetPostProcessingQuality ();
   return true;
}

// Set Video Quality
bool UROptionManager::SetVideoQualitySettings (const FRVideoQualitySetting& QualitySettings)
{
   UGameUserSettings* Settings = GetGameUserSettings ();
   if (!ensure (Settings)) return false;

   Settings->SetVSyncEnabled              (QualitySettings.VSyncEnabled      );
   Settings->SetFrameRateLimit            (QualitySettings.FrameRate         );
   Settings->SetResolutionScaleNormalized (QualitySettings.ResolutionQuality );
   Settings->SetDynamicResolutionEnabled  (QualitySettings.DynamicResolution );

   Settings->SetAntiAliasingQuality       (QualitySettings.AntiAliasing      );
   Settings->SetViewDistanceQuality       (QualitySettings.ViewDistance      );
   Settings->SetTextureQuality            (QualitySettings.TextureQuality    );
   Settings->SetShadowQuality             (QualitySettings.ShadowQuality     );
   Settings->SetShadingQuality            (QualitySettings.ShadingQuality    );
   Settings->SetGlobalIlluminationQuality (QualitySettings.GlobalIllumination);
   Settings->SetReflectionQuality         (QualitySettings.ReflectionQuality );
   Settings->SetFoliageQuality            (QualitySettings.FoliageQuality    );
   Settings->SetVisualEffectQuality       (QualitySettings.EffectQuality     );
   Settings->SetPostProcessingQuality     (QualitySettings.PostProcessQuality);

   Settings->ApplyNonResolutionSettings ();
   Settings->SaveSettings ();

   return true;
}


//=============================================================================
//                Audio Volume Settings
//=============================================================================

bool UROptionManager::GetGlobalSoundVolume (UObject* WorldContextObject, float &Volume)
{
   if (!ensure (WorldContextObject)) return false;
   const UWorld *world = WorldContextObject->GetWorld ();
   if (!ensure (world)) return false;

   FAudioDeviceHandle audioDeviceHandler = world->GetAudioDevice ();
   Volume = audioDeviceHandler->GetTransientPrimaryVolume ();
   return true;
}

bool UROptionManager::SetGlobalSoundVolume (UObject* WorldContextObject, const float NewVolume)
{
   if (!ensure (WorldContextObject)) return false;
   const UWorld *world = WorldContextObject->GetWorld ();
   if (!ensure (world)) return false;

   FAudioDeviceHandle audioDeviceHandler = world->GetAudioDevice ();
   audioDeviceHandler->SetTransientPrimaryVolume (NewVolume);
   return true;
}


//=============================================================================
//               Input Settings
//=============================================================================

bool UROptionManager::GetAllActionInput (TArray<FText>&InputActions, TArray<FText>&InputKeys)
{
   UInputSettings* settings = UInputSettings::GetInputSettings();
   if (!ensure (settings)) return false;

   InputActions.Empty();
   InputKeys.Empty();

   const TArray <FInputActionKeyMapping> &mapping = settings->GetActionMappings();
   for (auto map : mapping) {
      InputActions.Add (FText::FromName(map.ActionName));
      InputKeys.Add (map.Key.GetDisplayName());
   }
   return true;
}

bool UROptionManager::GetActionInput (const FName& ActionName, FText& ActionKey)
{
   UInputSettings* settings = UInputSettings::GetInputSettings();
   if (!ensure (settings)) return false;
   const TArray <FInputActionKeyMapping>& mapping = settings->GetActionMappings();
   for (auto map : mapping) {
      if (map.ActionName == ActionName) {
         ActionKey = map.Key.GetDisplayName ();
         return true;
      }
   }
   return false;
}

bool UROptionManager::SetActionInput (const FName& ActionName, const FText& ActionKey)
{
   UInputSettings* settings = UInputSettings::GetInputSettings();
   if (!ensure (settings)) return false;
   const TArray <FInputActionKeyMapping>& mapping = settings->GetActionMappings ();

   for (auto &map : mapping) {
      if (map.ActionName == ActionName) {
         FInputActionKeyMapping newAction = map;
         newAction.Key = FKey (*ActionKey.ToString());
         settings->RemoveActionMapping (map);
         settings->AddActionMapping (newAction);
      }
   }

   settings->SaveKeyMappings();
   for (TObjectIterator<UPlayerInput> It; It; ++It) {
      It->ForceRebuildingKeyMaps (true);
      It->TryUpdateDefaultConfigFile ();
   }
   return true;
}

