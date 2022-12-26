// Copyright 2015-2023 Vagen Ayrapetyan

#include "ROptionManager.h"
#include "AudioDevice.h"
#include "InputCore.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "GameFramework/InputSettings.h"
#include "GameFramework/GameUserSettings.h"

///////////////////////////////////////////////////////////////////////////////

//               Input Settings 

///////////////////////////////////////////////////////////////////////////////

void UROptionManager::GetAllActionInput(TArray<FText>&InputActions, TArray<FText>&InputKeys)
{
   UInputSettings* settings = UInputSettings::GetInputSettings();
   if (settings == NULL) return;

   InputActions.Empty();
   InputKeys.Empty();

   const TArray <FInputActionKeyMapping> &mapping = settings->GetActionMappings();
   for (auto map : mapping) {
      InputActions.Add(FText::FromName(map.ActionName));
      InputKeys.Add(map.Key.GetDisplayName());
   }
}

void UROptionManager::GetActionInput(const FName& ActionName, FText& ActionKey)
{
   UInputSettings* settings = UInputSettings::GetInputSettings();
   if (settings == NULL) return;
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
   if (settings == NULL) return;
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


///////////////////////////////////////////////////////////////////////////////

//                Audio Volume Settings

///////////////////////////////////////////////////////////////////////////////

void UROptionManager::GetGlobalSoundVolume(UObject* WorldContextObject, float &Volume) {
   if (WorldContextObject != nullptr){
      FAudioDeviceHandle audioDeviceHandler = WorldContextObject->GetWorld()->GetAudioDevice();
      Volume = audioDeviceHandler->GetTransientPrimaryVolume ();
   }
}

void UROptionManager::SetGlobalSoundVolume(UObject* WorldContextObject, const float NewVolume){
   if (WorldContextObject != nullptr) {
      FAudioDeviceHandle audioDeviceHandler = WorldContextObject->GetWorld()->GetAudioDevice();
      audioDeviceHandler->SetTransientPrimaryVolume (NewVolume);
   }
}

///////////////////////////////////////////////////////////////////////////////

//                Video Quality Settings

///////////////////////////////////////////////////////////////////////////////

// Get current video quality
bool UROptionManager::GetVideoQualitySettings(int32& ResolutionQuality,
                                             int32& ViewDistance,
                                             int32& AntiAliasing,
                                             int32& TextureQuality,
                                             int32& ShadowQuality,
                                             int32& EffectQuality,
                                             int32& PostProcessQuality)
{
   UGameUserSettings* Settings = GetGameUserSettings();
   if (!Settings) return false;

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
bool UROptionManager::SetVideoQualitySettings(const int32 ResolutionQuality,
                                             const int32 ViewDistance,
                                             const int32 AntiAliasing,
                                             const int32 TextureQuality,
                                             const int32 ShadowQuality,
                                             const int32 EffectQuality,
                                             const int32 PostProcessQuality)
{
   UGameUserSettings* Settings = GetGameUserSettings();
   if (!Settings) return false;

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
   UGameUserSettings* Settings = GetGameUserSettings();
   if (!Settings) return false;
   Settings->ConfirmVideoMode();
   Settings->ApplyNonResolutionSettings();
   Settings->SaveSettings();
   return true;
}

// Set the desired screen resolution(does not change it yet)
bool UROptionManager::SetScreenResolution(const int32 Width, const int32 Height, const bool Fullscreen)
{
   UGameUserSettings* Settings = GetGameUserSettings();
   if (!Settings) return false;

   Settings->SetScreenResolution(FIntPoint(Width, Height));
   Settings->SetFullscreenMode(Fullscreen ? EWindowMode::Fullscreen : EWindowMode::Windowed);
   return true;
}

// Change the current screen resolution
bool UROptionManager::ChangeScreenResolution(const int32 Width, const int32 Height, const bool Fullscreen)
{
   UGameUserSettings* Settings = GetGameUserSettings();
   if (!Settings) return false;
   
   EWindowMode::Type WindowMode = Fullscreen ? EWindowMode::Fullscreen : EWindowMode::Windowed;
   Settings->RequestResolutionChange(Width, Height, WindowMode, false);
   return true;
}



// Get List of Supported Resolutions
bool UROptionManager::GetSupportedScreenResolutions(TArray<FString>& Resolutions)
{
   FScreenResolutionArray ResolutionsArray;
   if (RHIGetAvailableResolutions(ResolutionsArray, true)){
      for (const FScreenResolutionRHI& Resolution : ResolutionsArray){
         FString StrW = FString::FromInt(Resolution.Width);
         FString StrH = FString::FromInt(Resolution.Height);
         Resolutions.AddUnique(StrW + "x" + StrH);
      }
      return true;
   }
   return false; 
}


// Get Screen Resolution
bool UROptionManager::GetScreenResolutions(int32 & Width, int32 & Height)
{
   UGameUserSettings* Settings = GetGameUserSettings();
   if (Settings) {
      FIntPoint TheResolution=Settings->GetScreenResolution();
      Width  = TheResolution.X;
      Height = TheResolution.Y;
      return true;
   }
   else return false;
}

// Get Game User Setting
UGameUserSettings* UROptionManager::GetGameUserSettings() {
   return (GEngine != nullptr ? GEngine->GameUserSettings : nullptr);
}