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
struct RMAPLIB_API FRMapPointInfo : public FTableRowBase
{
   GENERATED_BODY()

   // Associated with this actor
   UPROPERTY(EditAnywhere, BlueprintReadWrite)
      TSoftClassPtr<AActor> TargetClass;

   // Icon to render
   UPROPERTY(EditAnywhere, BlueprintReadWrite)
      TSoftObjectPtr<UTexture2D> Icon;

   // In pixels
   UPROPERTY(EditAnywhere, BlueprintReadWrite)
      float IconSize = 25;

   // Will appear on gameplay mini map
   UPROPERTY(EditAnywhere, BlueprintReadWrite)
      bool ShowOnMinimap = true;

   // Distance at which icon will become visible
   // 0 -> Always on minimap
   UPROPERTY(EditAnywhere, BlueprintReadWrite)
      float ShowOnMinimap_Range = 0;

   // Will appear on menu map
   UPROPERTY(EditAnywhere, BlueprintReadWrite)
      bool ShowOnMenumap = true;
};

UCLASS(ClassGroup=(_Rade))
class RMAPLIB_API URMapInfolLibrary : public UBlueprintFunctionLibrary
{
   GENERATED_BODY()
public:


};


