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

   URWorldInteractMgr ();

   //==========================================================================
   //                  Interact list
   //==========================================================================
private:
   // Container for current Interacts
   UPROPERTY()
      TArray<TWeakObjectPtr<URInteractComponent> > InteractList;
protected:
   UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Rade|Interact")
      const TArray<URInteractComponent*> GetInteractList () const;
public:
   // Called when Interact list has been modified
   UPROPERTY(BlueprintAssignable, Category = "Rade|Interact")
      FRInteractMgrEvent OnListUpdated;

   //==========================================================================
   //          Functions called by Interact components
   //==========================================================================

   UFUNCTION(BlueprintCallable, Category = "Rade|Interact")
      virtual void RegisterInteract (URInteractComponent* Interact);

   UFUNCTION(BlueprintCallable, Category = "Rade|Interact")
      virtual void UnregisterInteract  (URInteractComponent* Interact);

   //==========================================================================
   //                  Minimum search requirnments
   //==========================================================================

   // Distance from Player actor at which Interact can be searched
   UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Rade|Interact")
      float SearchDistanceMax = 400;

   // FOV angle of Camera to search Interacts
   UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Rade|Interact")
      float SearchAngleMax = 70;

   // Check if Interact can be Interacted
   UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Rade|Interact")
      bool IsValidInteract (
         const URInteractComponent *Interact,
         const FVector             &PlayerLocation,
         const FVector             &CameraLocation,
         const FVector             &CameraDirection) const;

   //==========================================================================
   //          Interact to point
   //==========================================================================

   // Angle in degrees to point
   UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Rade|Interact")
      FRuntimeFloatCurve SearchAnglePoint;

   // Distance to point
   UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Rade|Interact")
      FRuntimeFloatCurve SearchDistancePoint;

   // Calculate Interact point value
   UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Rade|Interact")
      float GetInteractPoint (
         const URInteractComponent *Interact,
         const FVector             &PlayerLocation,
         const FVector             &CameraLocation,
         const FVector             &CameraDirection) const;


   //==========================================================================
   //          Find Interact
   //==========================================================================

   // Find by world location and forward direction
   UFUNCTION(BlueprintCallable, Category = "Rade|Interact")
      virtual URInteractComponent* Find (
         const FVector &PlayerLocation,
         const FVector &CameraLocation,
         const FVector &CameraDirection);

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

