// Copyright 2015-2022 Vagen Ayrapetyan

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
class RADEINVENTORY_API ARItemPickup : public AActor
{
   GENERATED_BODY()

public:

   ARItemPickup (const class FObjectInitializer& PCIP);
   virtual void GetLifetimeReplicatedProps (TArray<FLifetimeProperty> &OutLifetimeProps) const override;
   virtual void BeginPlay () override;

   /////////////////////////////////////////////////////////////////////////////

   //          Components

   /////////////////////////////////////////////////////////////////////////////

   // Trigger Sphere 
   UPROPERTY(Replicated, VisibleAnywhere, BlueprintReadOnly, Category = "Rade|Inventory")
      USphereComponent* TriggerSphere;

   // Mesh
   UPROPERTY(Replicated, VisibleAnywhere, BlueprintReadOnly, Category = "Rade|Inventory")
      UMeshComponent* MeshComponent;

   //  Inventory Component
   UPROPERTY(Replicated, VisibleAnywhere, BlueprintReadOnly, Category = "Rade|Inventory")
      URInventoryComponent* Inventory;

   // Called if blueprint archetype of pickup was not defined;
   void InitEmpty ();

   /////////////////////////////////////////////////////////////////////////////

   //          Core properties

   /////////////////////////////////////////////////////////////////////////////

   // Activation delay
   UPROPERTY(Replicated, EditAnywhere, BlueprintReadWrite, Category = "Rade|Inventory")
      float PickupActivationDelay;

   // Auto Pickup
   UPROPERTY(Replicated, EditAnywhere, BlueprintReadWrite, Category = "Rade|Inventory")
      bool bAutoPickup;

   // Auto Pickup
   UPROPERTY(Replicated, EditAnywhere, BlueprintReadWrite, Category = "Rade|Inventory")
      bool bAutoDestroy;  


   /////////////////////////////////////////////////////////////////////////////

   //          Events

   /////////////////////////////////////////////////////////////////////////////

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
