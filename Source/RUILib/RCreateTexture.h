// Copyright 2015-2023 Vagen Ayrapetyan

#pragma once

#include "RUtilLib/RUtilTypes.h"
#include "Kismet/BlueprintAsyncActionBase.h"
#include "RCreateTexture.generated.h"

class UTexture2DDynamic;

class USceneCaptureComponent2D;

// ============================================================================
//                   Util Library
// ============================================================================

UCLASS()
class RUILIB_API URCreateTextureLibrary : public UBlueprintFunctionLibrary
{
   GENERATED_BODY()
public:

	UFUNCTION(BlueprintCallable,
             Category = "Rade|UI",
             meta = (HidePin      = "WorldContextObject",
                     WorldContext = "WorldContextObject"))
      static UTexture2D* CreateTextureSync (UObject* WorldContextObject,
                                            const TArray<uint8>& PixelData,
                                            int32 InSizeX = 0, int32 InSizeY = 0,
                                            EPixelFormat InPixelFormat = EPixelFormat::PF_B8G8R8A8);
};

// ============================================================================
//                   Async Task
// ============================================================================

UCLASS()
class RUILIB_API URCreateTextureAsync : public UBlueprintAsyncActionBase
{
	GENERATED_BODY()
public:


	UFUNCTION(BlueprintCallable,
             Category = "Rade|UI",
             meta = (BlueprintInternalUseOnly = "true",
                     HidePin      = "WorldContextObject",
                     WorldContext = "WorldContextObject"))
	   static URCreateTextureAsync* CreateTextureAsync (UObject* WorldContextObject,
                                                       const TArray<uint8>& PixelData,
                                                       int32 InSizeX = 0, int32 InSizeY = 0,
                                                       EPixelFormat InPixelFormat = EPixelFormat::PF_B8G8R8A8);



   // DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FRCreateTextureEvent, UTexture2D*, Texture);
   DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FRCreateTextureEvent, UTexture2DDynamic*, Texture);



   // Called when all save game slots have been read
	UPROPERTY(BlueprintAssignable)
	   FRCreateTextureEvent Loaded;

   // Execution point
	virtual void Activate () override;
private:

   UObject*      Outer = nullptr;
   TArray<uint8> PixelData;
   int32         SizeX = 0;
   int32         SizeY = 0;
   EPixelFormat  Format;
};

