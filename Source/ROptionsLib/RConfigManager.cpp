// Copyright 2015-2023 Vagen Ayrapetyan

#include "RConfigManager.h"

//=============================================================================
//                Config Settings
//=============================================================================

//             String
void URConfigManager::SaveConfig_String(const FString& FileName, const FString& SettingGroup, const FString& SettingName, const FString& SettingValue) {
   if (GConfig) GConfig->SetString(*SettingGroup, *SettingName, *SettingValue, GetConfigFullAddress(FileName));
}

void URConfigManager::LoadConfig_String(const FString& FileName, const FString& SettingGroup, const FString& SettingName, FString& SettingValue) {
   if (GConfig) GConfig->GetString(*SettingGroup, *SettingName, SettingValue, GetConfigFullAddress(FileName));
}

//             Int
void URConfigManager::SaveConfig_Int(const FString& FileName, const FString& SettingGroup, const FString& SettingName, const int32& SettingValue) {
   if (GConfig) GConfig->SetInt(*SettingGroup, *SettingName, SettingValue, GetConfigFullAddress(FileName));
}

void URConfigManager::LoadConfig_Int(const FString& FileName, const FString& SettingGroup, const FString& SettingName, int32& SettingValue) {
   if (GConfig) GConfig->GetInt(*SettingGroup, *SettingName, SettingValue, GetConfigFullAddress(FileName));
}

//             Float
void URConfigManager::SaveConfig_Float(const FString& FileName, const FString& SettingGroup, const FString& SettingName, const float& SettingValue) {
   if (GConfig) GConfig->SetFloat(*SettingGroup, *SettingName, SettingValue, GetConfigFullAddress(FileName));
}

void URConfigManager::LoadConfig_Float(const FString& FileName, const FString& SettingGroup, const FString& SettingName, float& SettingValue) {
   if (GConfig) GConfig->GetFloat(*SettingGroup, *SettingName, SettingValue, GetConfigFullAddress(FileName));
}

//             Vector
void URConfigManager::SaveConfig_Vector(const FString& FileName, const FString& SettingGroup, const FString& SettingName, const FVector& SettingValue) {
   if (GConfig) GConfig->SetVector(*SettingGroup, *SettingName, SettingValue, GetConfigFullAddress(FileName));
}

void URConfigManager::LoadConfig_Vector(const FString& FileName, const FString& SettingGroup, const FString& SettingName, FVector& SettingValue) {
   if (GConfig) GConfig->GetVector(*SettingGroup, *SettingName, SettingValue, GetConfigFullAddress(FileName));
}
//             Rotator
void URConfigManager::SaveConfig_Rotator(const FString& FileName, const FString& SettingGroup, const FString& SettingName, const FRotator& SettingValue) {
   if (GConfig) GConfig->SetRotator(*SettingGroup, *SettingName, SettingValue, GetConfigFullAddress(FileName));
}

void URConfigManager::LoadConfig_Rotator(const FString& FileName, const FString& SettingGroup, const FString& SettingName, FRotator& SettingValue) {
   if (GConfig) GConfig->GetRotator(*SettingGroup, *SettingName, SettingValue, GetConfigFullAddress(FileName));
}

//             Color
void URConfigManager::SaveConfig_Color(const FString& FileName, const FString& SettingGroup, const FString& SettingName, const FColor& SettingValue) {
   if (GConfig) GConfig->SetColor(*SettingGroup, *SettingName, SettingValue, GetConfigFullAddress(FileName));
}

void URConfigManager::LoadConfig_Color(const FString& FileName, const FString& SettingGroup, const FString& SettingName, FColor& SettingValue) {
   if (GConfig) GConfig->GetColor(*SettingGroup, *SettingName, SettingValue, GetConfigFullAddress(FileName));
}

void URConfigManager::ReloadConfig () {
   TArray<FString> configFiles;
   GConfig->GetConfigFilenames (configFiles);
   for (const FString& file : configFiles)
      GConfig->LoadFile (file);
}

FORCEINLINE FString URConfigManager::GetConfigFullAddress(const FString &FileName) {
   return ((FileName.Len() < 1 || FileName.Equals(" ")) ? (FPaths::ProjectConfigDir() + TEXT("DefaultGameUserSettings.ini")) :
      (FPaths::ProjectConfigDir () + FileName + TEXT(".ini")));
}

