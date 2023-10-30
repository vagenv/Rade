// Copyright 2015-2023 Vagen Ayrapetyan

#pragma once

#include "Components/ActorComponent.h"
#include "RWorldMapMgr.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE (FRMapMgrEvent);

class URMapPointComponent;

UCLASS(Blueprintable, BlueprintType, ClassGroup=(_Rade), meta=(BlueprintSpawnableComponent))
class RMAPLIB_API URWorldMapMgr : public UActorComponent
{
   GENERATED_BODY()
public:

   //==========================================================================
   //                  Interact list
   //==========================================================================
private:
   // Container all map actor
   UPROPERTY()
      TArray<TWeakObjectPtr<URMapPointComponent> > MapPointList;
protected:
   UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Rade|Interact")
      TArray<URMapPointComponent*> GetMapPointList () const;
public:
   // Called when map list has been modified
   UPROPERTY(BlueprintAssignable, Category = "Rade|Interact")
      FRMapMgrEvent OnListUpdated;

   //==========================================================================
   //          Functions called by Interact components
   //==========================================================================

   UFUNCTION(BlueprintCallable, Category = "Rade|Interact")
      virtual void RegisterMapPoint  (URMapPointComponent* MapActor);

   UFUNCTION(BlueprintCallable, Category = "Rade|Interact")
      virtual void UnregisterMapPoint (URMapPointComponent* MapActor);

   //==========================================================================
   //                  Get instance -> GameState component
   //==========================================================================

   UFUNCTION(BlueprintCallable, BlueprintPure,
             Category = "Rade|Map",
             meta = (HidePin          = "WorldContextObject",
                     DefaultToSelf    = "WorldContextObject",
                     DisplayName      = "World Map Mgr",
                     CompactNodeTitle = "World Map Mgr"))
      static URWorldMapMgr* GetInstance (const UObject* WorldContextObject);
};

