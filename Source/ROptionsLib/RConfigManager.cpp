// Copyright 2015-2023 Vagen Ayrapetyan

#include "RConfigManager.h"

//=============================================================================
//                Config Settings
//=============================================================================

// ---  String
bool URConfigManager::SaveConfig_String (const FString& FileName, const FString& SettingGroup, const FString& SettingName, const FString& SettingValue)
{
   if (!ensure (GConfig)) return false;
   GConfig->SetString (*SettingGroup, *SettingName, *SettingValue, GetConfigFullAddress (FileName));
   return true;
}

bool URConfigManager::LoadConfig_String (const FString& FileName, const FString& SettingGroup, const FString& SettingName, FString& SettingValue)
{
   if (!ensure (GConfig)) return false;
   GConfig->GetString (*SettingGroup, *SettingName, SettingValue, GetConfigFullAddress (FileName));
   return true;
}

// ---  Int
bool URConfigManager::SaveConfig_Int (const FString& FileName, const FString& SettingGroup, const FString& SettingName, const int32& SettingValue)
{
   if (!ensure (GConfig)) return false;
   GConfig->SetInt (*SettingGroup, *SettingName, SettingValue, GetConfigFullAddress (FileName));
   return true;
}

bool URConfigManager::LoadConfig_Int (const FString& FileName, const FString& SettingGroup, const FString& SettingName, int32& SettingValue)
{
   if (!ensure (GConfig)) return false;
   GConfig->GetInt (*SettingGroup, *SettingName, SettingValue, GetConfigFullAddress (FileName));
   return true;
}

// ---  Float
bool URConfigManager::SaveConfig_Float (const FString& FileName, const FString& SettingGroup, const FString& SettingName, const float& SettingValue)
{
   if (!ensure (GConfig)) return false;
   GConfig->SetFloat (*SettingGroup, *SettingName, SettingValue, GetConfigFullAddress (FileName));
   return true;
}

bool URConfigManager::LoadConfig_Float(const FString& FileName, const FString& SettingGroup, const FString& SettingName, float& SettingValue)
{
   if (!ensure (GConfig)) return false;
   GConfig->GetFloat (*SettingGroup, *SettingName, SettingValue, GetConfigFullAddress (FileName));
   return true;
}

// ---  Vector
bool URConfigManager::SaveConfig_Vector (const FString& FileName, const FString& SettingGroup, const FString& SettingName, const FVector& SettingValue)
{
   if (!ensure (GConfig)) return false;
   GConfig->SetVector (*SettingGroup, *SettingName, SettingValue, GetConfigFullAddress (FileName));
   return true;
}

bool URConfigManager::LoadConfig_Vector (const FString& FileName, const FString& SettingGroup, const FString& SettingName, FVector& SettingValue)
{
   if (!ensure (GConfig)) return false;
   GConfig->GetVector (*SettingGroup, *SettingName, SettingValue, GetConfigFullAddress (FileName));
   return true;
}

// ---  Rotator
bool URConfigManager::SaveConfig_Rotator (const FString& FileName, const FString& SettingGroup, const FString& SettingName, const FRotator& SettingValue)
{
   if (!ensure (GConfig)) return false;
   GConfig->SetRotator (*SettingGroup, *SettingName, SettingValue, GetConfigFullAddress (FileName));
   return true;
}

bool URConfigManager::LoadConfig_Rotator (const FString& FileName, const FString& SettingGroup, const FString& SettingName, FRotator& SettingValue)
{
   if (!ensure (GConfig)) return false;
   GConfig->GetRotator (*SettingGroup, *SettingName, SettingValue, GetConfigFullAddress (FileName));
   return true;
}

// ---  Color
bool URConfigManager::SaveConfig_Color (const FString& FileName, const FString& SettingGroup, const FString& SettingName, const FColor& SettingValue)
{
   if (!ensure (GConfig)) return false;
   GConfig->SetColor (*SettingGroup, *SettingName, SettingValue, GetConfigFullAddress (FileName));
   return true;
}

bool URConfigManager::LoadConfig_Color (const FString& FileName, const FString& SettingGroup, const FString& SettingName, FColor& SettingValue)
{
   if (!ensure (GConfig)) return false;
   GConfig->GetColor (*SettingGroup, *SettingName, SettingValue, GetConfigFullAddress (FileName));
   return true;
}

// ---  Reload
bool URConfigManager::ReloadConfig ()
{
   if (!ensure (GConfig)) return false;
   TArray<FString> ConfigFiles;
   GConfig->GetConfigFilenames (ConfigFiles);
   for (const FString& ItFile : ConfigFiles) GConfig->LoadFile (ItFile);
   return true;
}

FORCEINLINE FString URConfigManager::GetConfigFullAddress (const FString &FileName)
{
   return FileName.TrimChar (TEXT(' ')).IsEmpty ()
      ? FConfigCacheIni::NormalizeConfigIniPath ("DefaultGameUserSettings.ini")
      : FConfigCacheIni::NormalizeConfigIniPath (FileName + TEXT(".ini"));
}

