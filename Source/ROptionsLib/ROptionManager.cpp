// Copyright 2015-2023 Vagen Ayrapetyan

#include "ROptionManager.h"
#include "AudioDevice.h"
#include "InputCore.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "GameFramework/InputSettings.h"
#include "GameFramework/GameUserSettings.h"
#include "Math/UnrealMathUtility.h"


// Get Game User Setting
UGameUserSettings* UROptionManager::GetGameUserSettings ()
{
   return (GEngine != nullptr ? GEngine->GameUserSettings : nullptr);
}


bool FRScreenResolution::operator == (const FRScreenResolution &res) const noexcept
{
   return (Width == res.Width && Height == res.Height);
}

FString UROptionManager::ToString (const FRScreenResolution &Resolution)
{
   return FString::Printf (TEXT("%lldx%lld"), Resolution.Width, Resolution.Height);
}

bool UROptionManager::EqualEqual_FRScreenResolution (const FRScreenResolution& A, const FRScreenResolution& B)
{
   return A == B;
}

FRScreenResolution UROptionManager::ToResolution (const FString &Resolution)
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

bool UROptionManager::GetCurrentScreenResolution (FRScreenResolution &Resolutions)
{
   UGameUserSettings* Settings = GetGameUserSettings ();
   if (!ensure (Settings)) return false;

   FIntPoint res = Settings->GetScreenResolution ();
   Resolutions.Width = res.X;
   Resolutions.Height = res.Y;
   return true;
}


// Change the current screen resolution
bool UROptionManager::ChangeScreenResolution (const FRScreenResolution &Resolution, EWindowMode::Type WindowMode)
{
   UGameUserSettings* Settings = GetGameUserSettings ();
   if (!ensure (Settings)) return false;

   Settings->RequestResolutionChange (Resolution.Width, Resolution.Height, WindowMode, false);
   return true;
}



//==========================================================================//
//                Video Quality Settings
//==========================================================================//


// Get current video quality
bool UROptionManager::GetVideoQualitySettings (float& ResolutionQuality,
                                               int32& ViewDistance,
                                               int32& AntiAliasing,
                                               int32& TextureQuality,
                                               int32& ShadowQuality,
                                               int32& EffectQuality,
                                               int32& PostProcessQuality)
{
   UGameUserSettings* Settings = GetGameUserSettings ();
   if (!ensure (Settings)) return false;

   const Scalability::FQualityLevels &quality = Settings->ScalabilityQuality;
   ResolutionQuality  = quality.ResolutionQuality;
   ViewDistance       = quality.ViewDistanceQuality;
   AntiAliasing       = quality.AntiAliasingQuality;
   TextureQuality     = quality.TextureQuality;
   ShadowQuality      = quality.ShadowQuality;
   EffectQuality      = quality.EffectsQuality;
   PostProcessQuality = quality.PostProcessQuality;
   return true;
}

// Set Video Quality
bool UROptionManager::SetVideoQualitySettings (float ResolutionQuality,
                                               int32 ViewDistance,
                                               int32 AntiAliasing,
                                               int32 TextureQuality,
                                               int32 ShadowQuality,
                                               int32 EffectQuality,
                                               int32 PostProcessQuality)
{
   UGameUserSettings* Settings = GetGameUserSettings ();
   if (!ensure (Settings)) return false;

   Settings->ScalabilityQuality.ResolutionQuality   = ResolutionQuality;
   Settings->ScalabilityQuality.ViewDistanceQuality = ViewDistance;
   Settings->ScalabilityQuality.AntiAliasingQuality = AntiAliasing;
   Settings->ScalabilityQuality.TextureQuality      = TextureQuality;
   Settings->ScalabilityQuality.ShadowQuality       = ShadowQuality;
   Settings->ScalabilityQuality.EffectsQuality      = EffectQuality;
   Settings->ScalabilityQuality.PostProcessQuality  = PostProcessQuality;
   return true;
}

// Confirm and save current video mode (resolution and fullscreen/windowed)
bool UROptionManager::SaveVideoModeAndQuality()
{
   UGameUserSettings* Settings = GetGameUserSettings ();
   if (!ensure (Settings)) return false;
   Settings->ConfirmVideoMode ();
   Settings->ApplyNonResolutionSettings ();
   Settings->ApplySettings (false);
   Settings->SaveSettings ();
   return true;
}



//=============================================================================
//                Audio Volume Settings
//=============================================================================

void UROptionManager::GetGlobalSoundVolume (UObject* WorldContextObject, float &Volume)
{
   if (WorldContextObject != nullptr){
      FAudioDeviceHandle audioDeviceHandler = WorldContextObject->GetWorld ()->GetAudioDevice ();
      Volume = audioDeviceHandler->GetTransientPrimaryVolume ();
   }
}

void UROptionManager::SetGlobalSoundVolume (UObject* WorldContextObject, const float NewVolume)
{
   if (WorldContextObject != nullptr) {
      FAudioDeviceHandle audioDeviceHandler = WorldContextObject->GetWorld()->GetAudioDevice();
      audioDeviceHandler->SetTransientPrimaryVolume (NewVolume);
   }
}


//=============================================================================
//               Input Settings
//=============================================================================

void UROptionManager::GetAllActionInput(TArray<FText>&InputActions, TArray<FText>&InputKeys)
{
   UInputSettings* settings = UInputSettings::GetInputSettings();
   if (!ensure (settings)) return;

   InputActions.Empty();
   InputKeys.Empty();

   const TArray <FInputActionKeyMapping> &mapping = settings->GetActionMappings();
   for (auto map : mapping) {
      InputActions.Add (FText::FromName(map.ActionName));
      InputKeys.Add (map.Key.GetDisplayName());
   }
}

void UROptionManager::GetActionInput(const FName& ActionName, FText& ActionKey)
{
   UInputSettings* settings = UInputSettings::GetInputSettings();
   if (!ensure (settings)) return;
   const TArray <FInputActionKeyMapping>& mapping = settings->GetActionMappings();
   for (auto map : mapping) {
      if (map.ActionName == ActionName) {
         ActionKey = map.Key.GetDisplayName ();
         return;
      }
   }
}

void UROptionManager::SetActionInput(const FName& ActionName, const FText& ActionKey)
{
   UInputSettings* settings = UInputSettings::GetInputSettings();
   if (!ensure (settings)) return;
   const TArray <FInputActionKeyMapping>& mapping = settings->GetActionMappings();

   for (auto map : mapping) {
      if (map.ActionName == ActionName) {
         FInputActionKeyMapping newAction = map;
         newAction.Key = FKey (*ActionKey.ToString());
         settings->RemoveActionMapping(map);
         settings->AddActionMapping(newAction);
      }
   }

   settings->SaveKeyMappings();
   for (TObjectIterator<UPlayerInput> It; It; ++It) {
      It->ForceRebuildingKeyMaps(true);
      It->TryUpdateDefaultConfigFile ();
   }
}

