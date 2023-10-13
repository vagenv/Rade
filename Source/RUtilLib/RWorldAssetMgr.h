// Copyright 2015-2023 Vagen Ayrapetyan

#pragma once

#include "Components/ActorComponent.h"
#include "Engine/StreamableManager.h"
#include "RWorldAssetMgr.generated.h"

UCLASS(Blueprintable, BlueprintType, ClassGroup=(_Rade), meta=(BlueprintSpawnableComponent))
class RUTILLIB_API URWorldAssetMgr : public UActorComponent
{
   GENERATED_BODY()
public:

   FStreamableManager StreamableManager;

   //==========================================================================
   //                  Get instance -> GameState component
   //==========================================================================

   UFUNCTION(BlueprintCallable, BlueprintPure,
             Category = "Rade|Util",
             meta = (HidePin          = "WorldContextObject",
                     DefaultToSelf    = "WorldContextObject",
                     DisplayName      = "World Asset Mgr",
                     CompactNodeTitle = "World Asset Mgr"))
      static URWorldAssetMgr* GetInstance (const UObject* WorldContextObject);
};

