// Copyright 2015 Vagen Ayrapetyan

#include "OptionManager.h"
#include "../Rade.h"
#include "AudioDevice.h"
#include "InputCore.h"

//////////////////////////////////////////////////////////////////////////////////////////////////////////////

//               Input Settings 


void UOptionManager::GetAllActionInput(TArray<FText>&InputActions, TArray<FText>&InputKeys)
{
   UInputSettings* settings = UInputSettings::GetInputSettings();
   if (settings == NULL) return;

   InputActions.Empty();
   InputKeys.Empty();

   const TArray <FInputActionKeyMapping> &mapping = settings->GetActionMappings();
   for (auto map : mapping) {
      rlog (map.ActionName.ToString() + ":" + map.Key.ToString());
      InputActions.Add(FText::FromName(map.ActionName));
      InputKeys.Add(map.Key.GetDisplayName());
   }
}

void UOptionManager::GetActionInput(const FName& ActionName, FText& ActionKey)
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


void UOptionManager::SetActionInput(const FName& ActionName, const FText& ActionKey)
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
      It->UpdateDefaultConfigFile();
   }
}


///////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//                Config Settings

//             String
void UOptionManager::SaveConfig_String(const FString& FileName, const FString& SettingGroup, const FString& SettingName, const FString& SettingValue) {
   if (GConfig) GConfig->SetString(*SettingGroup, *SettingName, *SettingValue, GetConfigFullAddress(FileName));
}

void UOptionManager::LoadConfig_String(const FString& FileName, const FString& SettingGroup, const FString& SettingName, FString& SettingValue) {
   if (GConfig) GConfig->GetString(*SettingGroup, *SettingName, SettingValue, GetConfigFullAddress(FileName));
}

//             Int
void UOptionManager::SaveConfig_Int(const FString& FileName, const FString& SettingGroup, const FString& SettingName, const int32& SettingValue) {
   if (GConfig) GConfig->SetInt(*SettingGroup, *SettingName, SettingValue, GetConfigFullAddress(FileName));
}

void UOptionManager::LoadConfig_Int(const FString& FileName, const FString& SettingGroup, const FString& SettingName, int32& SettingValue) {
   if (GConfig) GConfig->GetInt(*SettingGroup, *SettingName, SettingValue, GetConfigFullAddress(FileName));
}

//             Float
void UOptionManager::SaveConfig_Float(const FString& FileName, const FString& SettingGroup, const FString& SettingName, const float& SettingValue) {
   if (GConfig) GConfig->SetFloat(*SettingGroup, *SettingName, SettingValue, GetConfigFullAddress(FileName));
}

void UOptionManager::LoadConfig_Float(const FString& FileName, const FString& SettingGroup, const FString& SettingName, float& SettingValue) {
   if (GConfig) GConfig->GetFloat(*SettingGroup, *SettingName, SettingValue, GetConfigFullAddress(FileName));
}

//             Vector
void UOptionManager::SaveConfig_Vector(const FString& FileName, const FString& SettingGroup, const FString& SettingName, const FVector& SettingValue) {
   if (GConfig) GConfig->SetVector(*SettingGroup, *SettingName, SettingValue, GetConfigFullAddress(FileName));
}

void UOptionManager::LoadConfig_Vector(const FString& FileName, const FString& SettingGroup, const FString& SettingName, FVector& SettingValue) {
   if (GConfig) GConfig->GetVector(*SettingGroup, *SettingName, SettingValue, GetConfigFullAddress(FileName));
}
//             Rotator
void UOptionManager::SaveConfig_Rotator(const FString& FileName, const FString& SettingGroup, const FString& SettingName, const FRotator& SettingValue) {
   if (GConfig) GConfig->SetRotator(*SettingGroup, *SettingName, SettingValue, GetConfigFullAddress(FileName));
}

void UOptionManager::LoadConfig_Rotator(const FString& FileName, const FString& SettingGroup, const FString& SettingName, FRotator& SettingValue) {
   if (GConfig) GConfig->GetRotator(*SettingGroup, *SettingName, SettingValue, GetConfigFullAddress(FileName));
}

//             Color
void UOptionManager::SaveConfig_Color(const FString& FileName, const FString& SettingGroup, const FString& SettingName, const FColor& SettingValue) {
   if (GConfig) GConfig->SetColor(*SettingGroup, *SettingName, SettingValue, GetConfigFullAddress(FileName));
}

void UOptionManager::LoadConfig_Color(const FString& FileName, const FString& SettingGroup, const FString& SettingName, FColor& SettingValue) {
   if (GConfig) GConfig->GetColor(*SettingGroup, *SettingName, SettingValue, GetConfigFullAddress(FileName));
}

void UOptionManager::ReloadConfig() {
   for (auto const& file : GConfig->GetFilenames())
      GConfig->LoadFile(file);
}

FORCEINLINE FString UOptionManager::GetConfigFullAddress(const FString &FileName) {
   return ((FileName.Len() < 1 || FileName.Equals(" ")) ? (FPaths::ProjectConfigDir() + TEXT("DefaultGameUserSettings.ini")) :
      (FPaths::ProjectConfigDir () + FileName + TEXT(".ini")));
}


///////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//                Audio Volume Settings


void UOptionManager::GetGlobalSoundVolume(UObject* WorldContextObject, float &Volume) {
   if (WorldContextObject != nullptr){
      FAudioDeviceHandle audioDeviceHandler = WorldContextObject->GetWorld()->GetAudioDevice();
      Volume = audioDeviceHandler->GetTransientMasterVolume ();
   }
}

void UOptionManager::SetGlobalSoundVolume(UObject* WorldContextObject, const float NewVolume){
   if (WorldContextObject != nullptr) {
      FAudioDeviceHandle audioDeviceHandler = WorldContextObject->GetWorld()->GetAudioDevice();
      audioDeviceHandler->SetTransientMasterVolume(NewVolume);
   }
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//                Video Quality Settings

// Get current video quality
bool UOptionManager::GetVideoQualitySettings(int32& ResolutionQuality,
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
bool UOptionManager::SetVideoQualitySettings(const int32 ResolutionQuality,
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
bool UOptionManager::SaveVideoModeAndQuality()
{
   UGameUserSettings* Settings = GetGameUserSettings();
   if (!Settings) return false;
   Settings->ConfirmVideoMode();
   Settings->ApplyNonResolutionSettings();
   Settings->SaveSettings();
   return true;
}

// Set the desired screen resolution(does not change it yet)
bool UOptionManager::SetScreenResolution(const int32 Width, const int32 Height, const bool Fullscreen)
{
   UGameUserSettings* Settings = GetGameUserSettings();
   if (!Settings) return false;

   Settings->SetScreenResolution(FIntPoint(Width, Height));
   Settings->SetFullscreenMode(Fullscreen ? EWindowMode::Fullscreen : EWindowMode::Windowed);
   return true;
}

// Change the current screen resolution
bool UOptionManager::ChangeScreenResolution(const int32 Width, const int32 Height, const bool Fullscreen)
{
   UGameUserSettings* Settings = GetGameUserSettings();
   if (!Settings) return false;
   
   EWindowMode::Type WindowMode = Fullscreen ? EWindowMode::Fullscreen : EWindowMode::Windowed;
   Settings->RequestResolutionChange(Width, Height, WindowMode, false);
   return true;
}


// Get List of Supported Resolutions
bool UOptionManager::GetSupportedScreenResolutions(TArray<FString>& Resolutions)
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
bool UOptionManager::GetScreenResolutions(int32 & Width, int32 & Height)
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
UGameUserSettings* UOptionManager::GetGameUserSettings() {
   return (GEngine != nullptr ? GEngine->GameUserSettings : nullptr);
}