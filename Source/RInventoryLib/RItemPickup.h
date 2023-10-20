// Copyright 2015-2023 Vagen Ayrapetyan

#pragma once

#include "GameFramework/Actor.h"
#include "RUILib/RUIDescription.h"
#include "RItemPickup.generated.h"

class USphereComponent;
class UMeshComponent;
class UPrimitiveComponent;
class AActor;
class URInventoryComponent;
class URInteractComponent;

DECLARE_DYNAMIC_MULTICAST_DELEGATE (FRItemPickupEvent);

// General pickup Class for Any Child of class "Item"
UCLASS(Blueprintable, BlueprintType, ClassGroup=(_Rade))
class RINVENTORYLIB_API ARItemPickup : public AActor,
                                       public IRGetDescriptionInterface
{
   GENERATED_BODY()

public:

   ARItemPickup ();
   virtual void GetLifetimeReplicatedProps (TArray<FLifetimeProperty> &OutLifetimeProps) const override;
   virtual void BeginPlay () override;

   //==========================================================================
   //          Components
   //==========================================================================

   // Mesh
   UPROPERTY(Replicated, VisibleAnywhere, BlueprintReadOnly, Category = "Rade|Inventory")
      TObjectPtr<UStaticMeshComponent> MeshComponent;

   //  Inventory Component
   UPROPERTY(Replicated, VisibleAnywhere, BlueprintReadOnly, Category = "Rade|Inventory")
      TObjectPtr<URInventoryComponent> Inventory;

   //  Interact Component
   UPROPERTY(Replicated, VisibleAnywhere, BlueprintReadOnly, Category = "Rade|Inventory")
      TObjectPtr<URInteractComponent> Interact;

   //==========================================================================
   //          Core properties
   //==========================================================================

   // Activation delay
   UPROPERTY(Replicated, EditDefaultsOnly, BlueprintReadOnly, Category = "Rade|Inventory")
      float PickupActivationDelay = 1.;

   // Auto Pickup
   UPROPERTY(Replicated, EditDefaultsOnly, BlueprintReadOnly, Category = "Rade|Inventory")
      bool bAutoDestroy = true;

protected:
   UPROPERTY(Replicated, EditDefaultsOnly, BlueprintReadOnly, Category = "Rade|Inventory")
      FRUIDescription Description;
public:
   virtual FRUIDescription GetDescription_Implementation () const override;

   //==========================================================================
   //          Events
   //==========================================================================

   // Activate Pickup Overlap Tracking
   void ActivatePickupOverlap ();

   //==========================================================================
   //          Blueprint Events
   //==========================================================================

   UPROPERTY(BlueprintAssignable, Category = "Rade|Inventory")
      FRItemPickupEvent OnPickupEmptied;

protected:

   // Inventory updated
   UFUNCTION()
      void OnInventoryUpdate ();
};

