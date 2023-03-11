// Copyright 2015-2023 Vagen Ayrapetyan

#pragma once

#include "RConfigManager.generated.h"

UCLASS(ClassGroup=(_Rade))
class ROPTIONSLIB_API URConfigManager : public UBlueprintFunctionLibrary
{
   GENERATED_BODY()
public:

   // Reload Config [Need to be done after every update of that config]
   UFUNCTION(BlueprintCallable, Category = "Rade|Settings")
      static bool ReloadConfig ();

   // Send FileName-> Recieve Full path in config folder
   FORCEINLINE static FString GetConfigFullAddress (const FString & FileName);


   //==========================================================================
   //                         Strings
   //==========================================================================

   // Save Config - String
   UFUNCTION(BlueprintCallable, Category = "Rade|Settings")
      static bool SaveConfig_String (const FString& FileName, const FString& SettingGroup, const FString& SettingName,const FString& SettingValue);

   // Load Config - String
   UFUNCTION(BlueprintCallable, Category = "Rade|Settings")
      static bool LoadConfig_String (const FString& FileName, const FString& SettingGroup, const FString& SettingName, FString& SettingValue);

   //==========================================================================
   //                         Int
   //==========================================================================

   // Save Config - Int
   UFUNCTION(BlueprintCallable, Category = "Rade|Settings")
      static bool SaveConfig_Int (const FString& FileName, const FString& SettingGroup, const FString& SettingName, const int32& SettingValue);

   // Load Config - Int
   UFUNCTION(BlueprintCallable, Category = "Rade|Settings")
      static bool LoadConfig_Int (const FString& FileName, const FString& SettingGroup, const FString& SettingName, int32& SettingValue);

   //==========================================================================
   //                        Float
   //==========================================================================

   // Save Config - Float
   UFUNCTION(BlueprintCallable, Category = "Rade|Settings")
      static bool SaveConfig_Float (const FString& FileName, const FString& SettingGroup, const FString& SettingName, const float& SettingValue);

   // Load Config - Float
   UFUNCTION(BlueprintCallable, Category = "Rade|Settings")
      static bool LoadConfig_Float (const FString& FileName, const FString& SettingGroup, const FString& SettingName, float& SettingValue);

   //==========================================================================
   //                         Vector
   //==========================================================================

   // Save Config - Vector
   UFUNCTION(BlueprintCallable, Category = "Rade|Settings")
      static bool SaveConfig_Vector (const FString& FileName, const FString& SettingGroup, const FString& SettingName, const FVector& SettingValue);

   // Load Config - Vector
   UFUNCTION(BlueprintCallable, Category = "Rade|Settings")
      static bool LoadConfig_Vector (const FString& FileName, const FString& SettingGroup, const FString& SettingName, FVector& SettingValue);

   //==========================================================================
   //                         Rotator
   //==========================================================================

   // Save Config - Rotator
   UFUNCTION(BlueprintCallable, Category = "Rade|Settings")
      static bool SaveConfig_Rotator (const FString& FileName, const FString& SettingGroup, const FString& SettingName, const FRotator& SettingValue);

   // Load Config - Rotator
   UFUNCTION(BlueprintCallable, Category = "Rade|Settings")
      static bool LoadConfig_Rotator (const FString& FileName, const FString& SettingGroup, const FString& SettingName, FRotator& SettingValue);

   //==========================================================================
   //                         Color
   //==========================================================================

   // Save Config - Color
   UFUNCTION(BlueprintCallable, Category = "Rade|Settings")
      static bool SaveConfig_Color (const FString& FileName, const FString& SettingGroup, const FString& SettingName, const FColor& SettingValue);

   // Load Config - Color
   UFUNCTION(BlueprintCallable, Category = "Rade|Settings")
      static bool LoadConfig_Color (const FString& FileName, const FString& SettingGroup, const FString& SettingName, FColor& SettingValue);
};

