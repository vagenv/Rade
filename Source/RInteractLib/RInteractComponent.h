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
   //         Can this Interact be found/selected/interacted
   //==========================================================================
public:

   UPROPERTY(BlueprintAssignable, Category = "Rade|Interact")
      FRInteractEvent OnIsInteractableUpdated;

   UFUNCTION(BlueprintCallable, Category = "Rade|Interact")
      void SetCanInteract (bool CanInteract_);

   UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Rade|Interact")
      bool GetCanInteract () const;
protected:

   // Can this Interact be selected
   UPROPERTY()
      bool CanInteract = true;

   //==========================================================================
   //         Is this Interact selected
   //==========================================================================
public:
   UPROPERTY(BlueprintAssignable, Category = "Rade|Interact")
      FRInteractEvent OnIsInteractedUpdated;

   UFUNCTION(BlueprintCallable, Category = "Rade|Interact")
      void SetIsInteracted (bool IsInteracted_);

   UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Rade|Interact")
      bool GetIsInteracted () const;

protected:

   // Is this Interact selected
   UPROPERTY()
      bool IsInteracted = false;
};

