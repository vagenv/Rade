// Copyright 2015-2023 Vagen Ayrapetyan

#pragma once

#include "CoreMinimal.h"
#include "Components/SceneComponent.h"
#include "RPlayerInteractMgr.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE (FRInteractingEvent);

class URInteractComponent;
class URWorldInteractMgr;

UCLASS(Blueprintable, BlueprintType, ClassGroup=(_Rade), meta=(BlueprintSpawnableComponent))
class RINTERACTLIB_API URPlayerInteractMgr : public USceneComponent
{
   GENERATED_BODY()
public:

   //==========================================================================
   //                         Core
   //==========================================================================

   URPlayerInteractMgr ();
   virtual void BeginPlay () override;
   virtual void EndPlay (const EEndPlayReason::Type EndPlayReason) override;


   UFUNCTION(BlueprintCallable, Category = "Rade|Interact")
      virtual void ForceRefreshInteractList ();

   //==========================================================================
   //                         World Mgr
   //==========================================================================
protected:
   UFUNCTION ()
      void FindWorldInteractMgr ();

   // Manager containing all available Interacts
   UPROPERTY()
      TWeakObjectPtr<URWorldInteractMgr> WorldInteractMgr = nullptr;

   //==========================================================================
   //                         Current interact
   //==========================================================================
protected:
   // Current focus Interact
   UPROPERTY()
      TWeakObjectPtr<URInteractComponent> InteractCurrent = nullptr;

   // Called when Interact is updated to broadcast event
   UFUNCTION()
      void ReportInteractCurrentUpdated () const;

   // Called by client to notify server
   UFUNCTION(Server, Reliable)
      void SetInteractCurrent_Server                (URInteractComponent* NewInteract);
      void SetInteractCurrent_Server_Implementation (URInteractComponent* NewInteract);

public:
   // Broadcasted when InteractCurrent has changed
   UPROPERTY(BlueprintAssignable, Category = "Rade|Interact")
      FRInteractingEvent OnCurrentInteractUpdated;

   UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Rade|Interact")
      virtual bool IsInteracting () const;

   UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Rade|Interact")
      virtual URInteractComponent* GetCurrentInteract () const;

protected:
  UFUNCTION()
      void UpdateInteractCurrent ();

   // Handle to UpdateInteractCurrent
   UPROPERTY()
      FTimerHandle UpdateInteractCurrentHandle;

   // How often should interact list be updated
   UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Rade|Interact")
      float UpdateInteractCurrentFrequency = 0.5;

   //==========================================================================
   //                         Interact List
   //==========================================================================
protected:

   // Current focus Interact
   UPROPERTY()
      TArray<URInteractComponent*> InteractList;

   UFUNCTION()
      void UpdateInteractList ();

   // Handle to UpdateInteractList
   UPROPERTY()
      FTimerHandle UpdateInteractListHandle;

   // How often should interact list be updated
   UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Rade|Interact")
      float UpdateInteractListFrequency = 4;

   // Distance for WorldInteractMgr
   UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Rade|Interact")
      float UpdateInteractListDistance = 2000;

public:

   UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Rade|Interact")
      TArray<URInteractComponent*> GetInteractList () const;

   //==========================================================================
   //                         Closest search
   //==========================================================================
protected:
   UFUNCTION(BlueprintCallable, Category = "Rade|Interact")
      URInteractComponent* FindClosestInteract () const;

   // Distance from Player actor at which Interact can be searched
   UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Rade|Interact")
      float CurrentDistanceMax = 500;

   // FOV angle of Camera to search Interacts
   UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Rade|Interact")
      float CurrentAngleMax = 70;

};

