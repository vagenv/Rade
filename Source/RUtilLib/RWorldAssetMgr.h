// Copyright 2015-2023 Vagen Ayrapetyan

#pragma once

#include "Components/ActorComponent.h"
#include "Engine/StreamableManager.h"
#include "RWorldAssetMgr.generated.h"

// Async load request container
struct RLoadAsyncRequest {
   FSoftObjectPath               Path;
   const UObject*                Requester;
   TSharedPtr<FStreamableHandle> Handle;
};

UCLASS(Blueprintable, BlueprintType, ClassGroup=(_Rade), meta=(BlueprintSpawnableComponent))
class RUTILLIB_API URWorldAssetMgr : public UActorComponent
{
   GENERATED_BODY()
public:

   // Container of all current async load requests
   TMap<FString, RLoadAsyncRequest> Requests;

   // Manager for requests
   FStreamableManager StreamableManager;

   // A conviniet interface to simply request chanining and management
   static void LoadAsync (const FSoftObjectPath &Path,
                          const UObject         *Requester,
                          TFunction<void(UObject *LoadedContent)>&& Callback);

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

