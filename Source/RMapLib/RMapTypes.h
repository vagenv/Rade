// Copyright 2015-2023 Vagen Ayrapetyan

#pragma once

//#include "RUILib/RUIDescription.h"
#include "Engine/DataTable.h"
#include "RMapTypes.generated.h"

class AActor;

// ============================================================================
//                   Map point info
// ============================================================================

USTRUCT(Blueprintable, BlueprintType)
struct RMAPLIB_API FRMapPointSetting
{
   GENERATED_BODY()

   // Show on map
   UPROPERTY(EditAnywhere, BlueprintReadWrite)
      bool Show = true;

   // If outside render bound, show it on border
   UPROPERTY(EditAnywhere, BlueprintReadWrite)
      bool AlwaysShow = false;

   // In pixels
   UPROPERTY(EditAnywhere, BlueprintReadWrite)
      float IconSize = 25;

   // false => constant IconSize
   UPROPERTY(EditAnywhere, BlueprintReadWrite)
      bool ScaleWithZoom = true;
};

USTRUCT(Blueprintable, BlueprintType)
struct RMAPLIB_API FRMapPointInfo : public FTableRowBase
{
   GENERATED_BODY()

   // Associated with this actor
   UPROPERTY(EditAnywhere, BlueprintReadWrite)
      TSoftClassPtr<AActor> TargetClass;

   // Icon to render
   UPROPERTY(EditAnywhere, BlueprintReadWrite)
      TSoftObjectPtr<UTexture2D> Icon;

   // Shown during gameplay
   UPROPERTY(EditAnywhere, BlueprintReadWrite)
      FRMapPointSetting MiniMap;

   // Large world map
   UPROPERTY(EditAnywhere, BlueprintReadWrite)
      FRMapPointSetting MenuMap;
};

UCLASS(ClassGroup=(_Rade))
class RMAPLIB_API URMapInfolLibrary : public UBlueprintFunctionLibrary
{
   GENERATED_BODY()
public:


};


