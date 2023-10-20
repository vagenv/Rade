// Copyright 2015-2023 Vagen Ayrapetyan

#pragma once

#include "Components/SceneComponent.h"
#include "RInteractComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE (FRInteractEvent);

UCLASS(Blueprintable, BlueprintType, ClassGroup=(_Rade), meta=(BlueprintSpawnableComponent))
class RINTERACTLIB_API URInteractComponent : public USceneComponent
{
   GENERATED_BODY()
public:

   virtual void BeginPlay () override;
   virtual void EndPlay (const EEndPlayReason::Type EndPlayReason) override;

protected:

   void RegisterInteract ();
   void UnregisterInteract ();

   //==========================================================================
   //         Can this Interact be selected
   //==========================================================================
public:

   UPROPERTY(BlueprintAssignable, Category = "Rade|Interact")
      FRInteractEvent OnIsInteractableUpdated;

   UFUNCTION(BlueprintCallable, Category = "Rade|Interact")
      void SetIsInteractable (bool CanFind);

   UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Rade|Interact")
      bool GetIsInteractable () const;

   //==========================================================================
   //         Is this Interact selected
   //==========================================================================

   UPROPERTY(BlueprintAssignable, Category = "Rade|Interact")
      FRInteractEvent OnIsInteractedUpdated;

   UFUNCTION(BlueprintCallable, Category = "Rade|Interact")
      void SetIsInteracted (bool CanInteract);

   UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Rade|Interact")
      bool GetIsInteracted () const;

protected:

   // Is this Interact selected
   UPROPERTY()
      bool IsInteracted = false;

   // Can this Interact be selected
   UPROPERTY()
      bool IsInteractable = true;
};

