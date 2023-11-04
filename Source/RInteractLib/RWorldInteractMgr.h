// Copyright 2015-2023 Vagen Ayrapetyan

#pragma once

#include "Components/ActorComponent.h"
#include "RWorldInteractMgr.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE (FRInteractMgrEvent);

class URInteractComponent;

UCLASS(Blueprintable, BlueprintType, ClassGroup=(_Rade), meta=(BlueprintSpawnableComponent))
class RINTERACTLIB_API URWorldInteractMgr : public UActorComponent
{
   GENERATED_BODY()
public:

   //==========================================================================
   //                  Interact list
   //==========================================================================
private:
   // Container for current Interacts
   UPROPERTY()
      TArray<TWeakObjectPtr<URInteractComponent> > InteractList;
protected:
   UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Rade|Interact")
      TArray<URInteractComponent*> GetInteractList () const;
public:
   // Called when Interact list has been modified
   UPROPERTY(BlueprintAssignable, Category = "Rade|Interact")
      FRInteractMgrEvent OnListUpdated;

   UFUNCTION()
      virtual void ReportListUpdateDelayed ();

   bool ReportListUpdatedDelayedTriggered = false;

   //==========================================================================
   //          Functions called by Interact components
   //==========================================================================

   UFUNCTION(BlueprintCallable, Category = "Rade|Interact")
      virtual void RegisterInteract   (URInteractComponent* Interact);

   UFUNCTION(BlueprintCallable, Category = "Rade|Interact")
      virtual void UnregisterInteract (URInteractComponent* Interact);

   //==========================================================================
   //         Find Interact
   //==========================================================================

   // Get list of interactables in distance
   UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Rade|Interact")
      TArray<URInteractComponent*> Find (const FVector &Location, float Distance) const;

   //==========================================================================
   //                  Get instance -> GameState component
   //==========================================================================

   UFUNCTION(BlueprintCallable, BlueprintPure,
             Category = "Rade|Interact",
             meta = (HidePin          = "WorldContextObject",
                     DefaultToSelf    = "WorldContextObject",
                     DisplayName      = "World Interact Mgr",
                     CompactNodeTitle = "World Interact Mgr"))
      static URWorldInteractMgr* GetInstance (const UObject* WorldContextObject);
};

