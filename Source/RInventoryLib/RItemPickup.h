// Copyright 2015-2023 Vagen Ayrapetyan

#pragma once

#include "GameFramework/Actor.h"
#include "RItemTypes.h"
#include "RItemPickup.generated.h"

class USphereComponent;
class UMeshComponent;
class UPrimitiveComponent;
class AActor;

class URInventoryComponent;

// General pickup Class for Any Child of class "Item"
UCLASS(Blueprintable)
class RINVENTORYLIB_API ARItemPickup : public AActor
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
   UPROPERTY(Replicated, EditAnywhere, BlueprintReadWrite, Category = "Rade|Inventory")
      TObjectPtr<UStaticMeshComponent> MeshComponent;

   // Trigger Sphere
   UPROPERTY(Replicated, EditAnywhere, BlueprintReadWrite, Category = "Rade|Inventory")
      TObjectPtr<USphereComponent> TriggerSphere;

   //  Inventory Component
   UPROPERTY(Replicated, EditAnywhere, BlueprintReadWrite, Category = "Rade|Inventory")
      TObjectPtr<URInventoryComponent> Inventory;

   //==========================================================================
   //          Core properties
   //==========================================================================

   // Activation delay
   UPROPERTY(Replicated, EditAnywhere, BlueprintReadWrite, Category = "Rade|Inventory")
      float PickupActivationDelay = 1.;

   // Auto Pickup
   UPROPERTY(Replicated, EditAnywhere, BlueprintReadWrite, Category = "Rade|Inventory")
      bool bAutoPickup = false;

   // Auto Pickup
   UPROPERTY(Replicated, EditAnywhere, BlueprintReadWrite, Category = "Rade|Inventory")
      bool bAutoDestroy = true;


   //==========================================================================
   //          Events
   //==========================================================================

   // Item was picked up by player
   void PickedUp (AActor* InventoryOwner);

   // BP Event that item Was Picked Up
   UFUNCTION(BlueprintImplementableEvent, Category = "Rade|Inventory")
      void BP_PickedUp (AActor* InventoryOwner);

   // BP Event that Player Entered Pickup Area
   UFUNCTION(BlueprintImplementableEvent, Category = "Rade|Inventory")
      void BP_PlayerEntered (AActor* InventoryOwner);

   // BP Event that Player Exited Pickup Area
   UFUNCTION(BlueprintImplementableEvent, Category = "Rade|Inventory")
      void BP_PlayerLeft (AActor* InventoryOwner);

   // Activate Pickup Overlap Tracking
   void ActivatePickupOverlap ();

   // Activate Pickup Physics
   void ActivatePickupPhysics ();

protected:

   // Player Entered
   UFUNCTION()
      void OnBeginOverlap (UPrimitiveComponent* OverlappedComponent,
                           AActor* OtherActor,
                           UPrimitiveComponent* OtherComp,
                           int32 OtherBodyIndex,
                           bool bFromSweep,
                           const FHitResult & SweepResult);

   // Player Exited
   UFUNCTION()
      void OnEndOverlap (UPrimitiveComponent* OverlappedComponent,
                         AActor* OtherActor,
                         UPrimitiveComponent* OtherComp,
                         int32 OtherBodyIndex);

   // Inventory updated
   UFUNCTION()
      void OnInventoryUpdate ();

};

