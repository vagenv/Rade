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
public:
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Rade|Interact")
      virtual bool IsInteracting () const;

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Rade|Interact")
      virtual URInteractComponent* GetCurrentInteract () const;

   // Broadcasted when InteractCurrent has changed
   UPROPERTY(BlueprintAssignable, Category = "Rade|Interact")
      FRInteractingEvent OnInteractUpdated;

protected:
	// Current focus Interact
   UPROPERTY()
      TWeakObjectPtr<URInteractComponent> InteractCurrent = nullptr;

   //==========================================================================
   //                         Interact search
   //==========================================================================
protected:

   // How often should interact target be searched. In Seconds.
   UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Rade|Interact")
      float SearchFrequency = 1;

   // Handle to InteractCheck
   UPROPERTY()
      FTimerHandle InteractCheckHandle;

   // Search/Checks if Interacting actor is valid and within range
   UFUNCTION()
      virtual void InteractCheck ();

   // Called by client to notify server
   UFUNCTION(Server, Reliable)
      void SetInteractCurrent_Server                (URInteractComponent* NewInteract);
      void SetInteractCurrent_Server_Implementation (URInteractComponent* NewInteract);

   // Called when Interact is updated to broadcast event
   UFUNCTION()
      void ReportInteractUpdate () const;

   //==========================================================================
   //                         Should search
   //==========================================================================
/*
protected:
   UFUNCTION()
      virtual void SetInteractCheckEnabled (bool Enabled);

   bool ShouldSearch = true;
*/
};

