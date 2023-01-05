// Copyright 2015-2023 Vagen Ayrapetyan

#pragma once

#include "RItemTypes.h"
#include "Components/ActorComponent.h"
#include "RInventoryComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE (FRInventoryEvent);

class ARItemPickup;
class URInventoryComponent;

// Inventory Component. Holds all items an actor own
UCLASS(Blueprintable, BlueprintType)
class RINVENTORYLIB_API URInventoryComponent : public UActorComponent
{
   GENERATED_BODY()
public:

   // Base events
   URInventoryComponent ();
   virtual void GetLifetimeReplicatedProps (TArray<FLifetimeProperty> &OutLifetimeProps) const override;
   virtual void BeginPlay () override;
   virtual void EndPlay (const EEndPlayReason::Type EndPlayReason) override;

private:
      // Has authority to change inventory
      bool bIsServer = false;
public:

   //=============================================================================
   //                 Inventory info
   //=============================================================================

   // Maximum number
   UPROPERTY(Replicated, EditAnywhere, BlueprintReadWrite, Category = "Rade|Inventory")
      int32 SlotsMax = 10;

   // Current
   UPROPERTY(Replicated, VisibleAnywhere, BlueprintReadOnly, Category = "Rade|Inventory")
      float WeightCurrent = 0;

   // Maximum weight actor can carry
   UPROPERTY(Replicated, EditAnywhere, BlueprintReadWrite, Category = "Rade|Inventory")
      float WeightMax = 25;

   //=============================================================================
   //                 Item list
   //=============================================================================

   UFUNCTION(BlueprintPure, Category = "Rade|Inventory")
      TArray<FRItemData> GetItems () const;

protected:

   // Network call when Item list has been updated
   UFUNCTION()
      void OnRep_Items ();

   UPROPERTY(ReplicatedUsing = "OnRep_Items", Replicated, EditAnywhere, BlueprintReadWrite, Category = "Rade|Inventory")
      TArray<FRItemData> Items;


   void CalcWeight ();

public:

   // Item to be added upon game start
   UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Rade|Inventory")
      TArray<FRDefaultItem> DefaultItems;

   // Delegate when Item list updated
   UPROPERTY(BlueprintAssignable, Category = "Rade|Inventory")
      FRInventoryEvent OnInventoryUpdated;

   //=============================================================================
   //                 Add/Remove to Inventory
   //=============================================================================

   // --- Add Default item
   UFUNCTION(BlueprintCallable, Category = "Rade|Inventory")
      bool AddItem_Arch (const FRDefaultItem &Item);

   // --- Add Item struct
   UFUNCTION(Server, Reliable, BlueprintCallable, Category = "Rade|Inventory")
      void AddItem_Server                (FRItemData ItemData);
      void AddItem_Server_Implementation (FRItemData ItemData);
   UFUNCTION(                  BlueprintCallable, Category = "Rade|Inventory")
      bool AddItem                       (FRItemData ItemData);

   // --- Remove item
   UFUNCTION(Server, Reliable, BlueprintCallable, Category = "Rade|Inventory")
      void RemoveItem_Server                (int32 ItemIdx, int32 Count = 1);
      void RemoveItem_Server_Implementation (int32 ItemIdx, int32 Count = 1);
   UFUNCTION(                 BlueprintCallable, Category = "Rade|Inventory")
      bool RemoveItem                       (int32 ItemIdx, int32 Count = 1);

   // --- Transfer everything
   UFUNCTION(Server, Reliable, BlueprintCallable, Category = "Rade|Inventory")
      void TransferAll_Server                (URInventoryComponent *DstInventory);
      void TransferAll_Server_Implementation (URInventoryComponent *DstInventory);
   UFUNCTION(                  BlueprintCallable, Category = "Rade|Inventory")
      bool TransferAll                       (URInventoryComponent *DstInventory);

   // --- Transfer a single item
   UFUNCTION(Server, Reliable, BlueprintCallable, Category = "Rade|Inventory")
      void TransferItem_Server                (URInventoryComponent *DstInventory,
                                               int32 SrcItemIdx,
                                               int32 SrcItemCount);
      void TransferItem_Server_Implementation (URInventoryComponent *DstInventory,
                                               int32 SrcItemIdx,
                                               int32 SrcItemCount);
   UFUNCTION(                  BlueprintCallable, Category = "Rade|Inventory")
      bool TransferItem                        (URInventoryComponent *DstInventory,
                                                int32 SrcItemIdx,
                                                int32 SrcItemCount);

   //=============================================================================
   //                 Item use / drop events
   //=============================================================================

   UFUNCTION(Server, Reliable, BlueprintCallable, Category = "Rade|Inventory")
      void UseItem_Server                (int32 ItemIdx);
      void UseItem_Server_Implementation (int32 ItemIdx);
   UFUNCTION(                  BlueprintCallable, Category = "Rade|Inventory")
      bool UseItem                       (int32 ItemIdx);

   UFUNCTION(Server, Reliable, BlueprintCallable, Category = "Rade|Inventory")
      void DropItem_Server                (int32 ItemIdx, int32 Count = 0);
      void DropItem_Server_Implementation (int32 ItemIdx, int32 Count = 0);
   UFUNCTION(                  BlueprintCallable, Category = "Rade|Inventory")
      class ARItemPickup* DropItem        (int32 ItemIdx, int32 Count = 0);

   //=============================================================================
   //                 Events
   //=============================================================================

   UFUNCTION(BlueprintImplementableEvent, Category = "Rade|Item")
      void BP_Used (int32 ItemIdx);

   UFUNCTION(BlueprintImplementableEvent, Category = "Rade|Item")
      void BP_Droped (int32 ItemIdx, ARItemPickup *Pickup);

   //=============================================================================
   //                 Pickups
   //=============================================================================

protected:
   // List of currently available pickup
   UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Rade|Inventory",
             meta = (GetByRef))
      TArray<const ARItemPickup*> CurrentPickups;

   UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Rade|Inventory")
      const ARItemPickup* ClosestPickup = nullptr;

   // Delegate when pickup list updated
   UPROPERTY(BlueprintAssignable, Category = "Rade|Inventory")
      FRInventoryEvent OnPickupListUpdated;

   UPROPERTY(BlueprintAssignable, Category = "Rade|Inventory")
      FRInventoryEvent OnClosestPickupUpdated;

   FTimerHandle TimerClosestPickup;

   UFUNCTION()
      void CheckClosestPickup ();

public:

   // // Network call when Item list has been updated
   // UFUNCTION()
   //    void OnRep_CurrentPickups ();

   // Owner overlaps pickup
   bool Pickup_Add (const ARItemPickup* Pickup);

   // Owner no longer overlaps
   bool Pickup_Rm  (const ARItemPickup* Pickup);

   UFUNCTION(BlueprintPure, Category = "Rade|Inventory")
      const ARItemPickup* GetClosestPickup () const;

   // Should be used only for main local Player.
   UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Rade|Inventory")
      bool bCheckClosestPickup = false;

   UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Rade|Inventory")
      float CheckClosestDelay = 0.5f;

   //=============================================================================
   //                 Save / Load
   //=============================================================================

public:
   // Inventory Saved / Loaded between sessions.
   // Should be used only for Player.
   // Careful with collision of 'InventoryUniqueId'
   UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Rade|Inventory")
      bool bSaveLoadInventory = false;

protected:
   // Rade Save events
   UFUNCTION()
      void OnSave ();
   UFUNCTION()
      void OnLoad ();
};

