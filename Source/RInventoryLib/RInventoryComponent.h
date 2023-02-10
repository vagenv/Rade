// Copyright 2015-2023 Vagen Ayrapetyan

#pragma once

#include "RItemTypes.h"
#include "Components/ActorComponent.h"
#include "RInventoryComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE (FRInventoryEvent);

class ARItemPickup;

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

protected:
      // Has authority to change inventory
      bool bIsServer = false;
public:

   //==========================================================================
   //                 Inventory info
   //==========================================================================

   // Maximum number
   UPROPERTY(Replicated, EditAnywhere, BlueprintReadWrite, Category = "Rade|Inventory")
      int32 SlotsMax = 10;

   // Current
   UPROPERTY(Replicated, VisibleAnywhere, BlueprintReadOnly, Category = "Rade|Inventory")
      float WeightCurrent = 0;

   // Maximum weight actor can carry
   UPROPERTY(Replicated, EditAnywhere, BlueprintReadWrite, Category = "Rade|Inventory")
      float WeightMax = 25;

   //==========================================================================
   //                 Item list
   //==========================================================================

   UFUNCTION(BlueprintPure, Category = "Rade|Inventory")
      TArray<FRItemData> GetItems () const;

protected:

   // Network call when Item list has been updated
   UFUNCTION()
      void OnRep_Items ();

   UPROPERTY(ReplicatedUsing = "OnRep_Items", Replicated, VisibleAnywhere, BlueprintReadOnly, Category = "Rade|Inventory")
      TArray<FRItemData> Items;

   void CalcWeight ();

public:

   // Item to be added upon game start
   UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Rade|Inventory")
      TArray<FRItemDataHandle> DefaultItems;

   // Delegate when Item list updated
   UPROPERTY(BlueprintAssignable, Category = "Rade|Inventory")
      FRInventoryEvent OnInventoryUpdated;

   //==========================================================================
   //                 Check if contains
   //==========================================================================

   // --- Check if Inventory contains.
   //     Convenience functions

   UFUNCTION(BlueprintPure, Category = "Rade|Inventory")
      bool HasItem_Arch (const FRItemDataHandle &CheckItem) const;

   UFUNCTION(BlueprintPure, Category = "Rade|Inventory")
      bool HasItem_Data (FRItemData CheckItem) const;

   UFUNCTION(BlueprintPure, Category = "Rade|Inventory")
      bool HasItems (const TArray<FRItemDataHandle> &CheckItems) const;

   //==========================================================================
   //                 Get item count
   //==========================================================================

   UFUNCTION(BlueprintPure, Category = "Rade|Inventory")
      int GetCountItem (const FRItemData &CheckItem) const;

   UFUNCTION(BlueprintPure, Category = "Rade|Inventory")
      int GetCountItem_Name (const FString &CheckItemname) const;

   //==========================================================================
   //                 Add to Inventory
   //==========================================================================

   // --- Add Default item
   UFUNCTION(BlueprintCallable, Category = "Rade|Inventory")
      bool AddItem_Arch (const FRItemDataHandle &Item);

   // --- Add Item struct
   UFUNCTION(Server, Reliable, BlueprintCallable, Category = "Rade|Inventory")
      void AddItem_Server                (URInventoryComponent *DstInventory,
                                          FRItemData ItemData) const;
      void AddItem_Server_Implementation (URInventoryComponent *DstInventory,
                                          FRItemData ItemData) const;
   UFUNCTION(                  BlueprintCallable, Category = "Rade|Inventory")
      bool AddItem                       (FRItemData ItemData);

   //==========================================================================
   //                 Remove from Inventory
   //==========================================================================

   // --- Remove item index
   UFUNCTION(Server, Reliable, BlueprintCallable, Category = "Rade|Inventory")
      void RemoveItem_Index_Server                (URInventoryComponent *SrcInventory,
                                                   int32 ItemIdx, int32 Count = 1) const;
      void RemoveItem_Index_Server_Implementation (URInventoryComponent *SrcInventory,
                                                   int32 ItemIdx, int32 Count = 1) const;
   UFUNCTION(                 BlueprintCallable, Category = "Rade|Inventory")
      bool RemoveItem_Index                       (int32 ItemIdx, int32 Count = 1);

   // --- Remove item arch
   UFUNCTION(Server, Reliable, BlueprintCallable, Category = "Rade|Inventory")
      void RemoveItem_Arch_Server                (URInventoryComponent *SrcInventory,
                                                  const FRItemDataHandle &ItemHandle) const;
      void RemoveItem_Arch_Server_Implementation (URInventoryComponent *SrcInventory,
                                                  const FRItemDataHandle &ItemHandle) const;
   UFUNCTION(                 BlueprintCallable, Category = "Rade|Inventory")
      bool RemoveItem_Arch                       (const FRItemDataHandle &ItemHandle);


   // --- Remove item arch
   UFUNCTION(Server, Reliable, BlueprintCallable, Category = "Rade|Inventory")
      void RemoveItem_Data_Server                (URInventoryComponent *SrcInventory,
                                                  FRItemData ItemData) const;
      void RemoveItem_Data_Server_Implementation (URInventoryComponent *SrcInventory,
                                                  FRItemData ItemData) const;
   UFUNCTION(                 BlueprintCallable, Category = "Rade|Inventory")
      bool RemoveItem_Data                       (FRItemData ItemData);

   //==========================================================================
   //                 Transfer between Inventories
   //==========================================================================

   // --- Transfer everything
   UFUNCTION(Server, Reliable, BlueprintCallable, Category = "Rade|Inventory")
      void TransferAll_Server                (URInventoryComponent *SrcInventory,
                                              URInventoryComponent *DstInventory) const;
      void TransferAll_Server_Implementation (URInventoryComponent *SrcInventory,
                                              URInventoryComponent *DstInventory) const;
   UFUNCTION(                  BlueprintCallable, Category = "Rade|Inventory")
      bool TransferAll                       (URInventoryComponent *DstInventory);

   // --- Transfer a single item
   UFUNCTION(Server, Reliable, BlueprintCallable, Category = "Rade|Inventory")
      void TransferItem_Server                (URInventoryComponent *SrcInventory,
                                               URInventoryComponent *DstInventory,
                                               int32 SrcItemIdx,
                                               int32 SrcItemCount) const;
      void TransferItem_Server_Implementation (URInventoryComponent *SrcInventory,
                                               URInventoryComponent *DstInventory,
                                               int32 SrcItemIdx,
                                               int32 SrcItemCount) const;
   UFUNCTION(                  BlueprintCallable, Category = "Rade|Inventory")
      bool TransferItem                        (URInventoryComponent *DstInventory,
                                                int32 SrcItemIdx,
                                                int32 SrcItemCount);

   //==========================================================================
   //                 Item use / drop events
   //==========================================================================

   UFUNCTION(Server, Reliable, BlueprintCallable, Category = "Rade|Inventory")
      void UseItem_Server                (URInventoryComponent *SrcInventory,
                                          int32 ItemIdx) const;
      void UseItem_Server_Implementation (URInventoryComponent *SrcInventory,
                                          int32 ItemIdx) const;
   UFUNCTION(                  BlueprintCallable, Category = "Rade|Inventory")
      virtual bool UseItem               (int32 ItemIdx);

   UFUNCTION(Server, Reliable, BlueprintCallable, Category = "Rade|Inventory")
      void DropItem_Server                (URInventoryComponent *SrcInventory,
                                           int32 ItemIdx, int32 Count = 0) const;
      void DropItem_Server_Implementation (URInventoryComponent *SrcInventory,
                                           int32 ItemIdx, int32 Count = 0) const;
   UFUNCTION(                  BlueprintCallable, Category = "Rade|Inventory")
      class ARItemPickup* DropItem        (int32 ItemIdx, int32 Count = 0);

   //==========================================================================
   //                 Events
   //==========================================================================

   UFUNCTION(BlueprintImplementableEvent, Category = "Rade|Item")
      void BP_Used (int32 ItemIdx);

   UFUNCTION(BlueprintImplementableEvent, Category = "Rade|Item")
      void BP_Droped (int32 ItemIdx, ARItemPickup *Pickup);

   //==========================================================================
   //                 Pickups
   //==========================================================================

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

   //==========================================================================
   //                 Save / Load
   //==========================================================================

public:
   // Inventory Saved / Loaded between sessions.
   // Should be used only for Player.
   // Careful with collision of 'InventoryUniqueId'
   UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Rade|Inventory")
      bool bSaveLoad = false;

protected:
   // Rade Save events
   UFUNCTION()
      void OnSave ();
   UFUNCTION()
      void OnLoad ();
};

