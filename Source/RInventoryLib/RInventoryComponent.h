// Copyright 2015-2023 Vagen Ayrapetyan

#pragma once

#include "RItemTypes.h"
#include "Components/ActorComponent.h"
#include "RSaveLib/RSaveInterface.h"
#include "RInventoryComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE (FRInventoryEvent);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam (FRInventoryItemListEvent,
                                             const TArray<FRItemData>&, ItemDataList);

// Inventory Component. Holds all items an actor own
UCLASS(Blueprintable, BlueprintType, ClassGroup=(_Rade), meta=(BlueprintSpawnableComponent))
class RINVENTORYLIB_API URInventoryComponent : public UActorComponent, public IRSaveInterface
{
   GENERATED_BODY()
public:

   // Base events
   URInventoryComponent ();
   virtual void GetLifetimeReplicatedProps (TArray<FLifetimeProperty> &OutLifetimeProps) const override;
   virtual void BeginPlay () override;

   //==========================================================================
   //                 Inventory info
   //==========================================================================

   // Maximum number
   UPROPERTY(Replicated, EditDefaultsOnly, BlueprintReadOnly, Category = "Rade|Inventory")
      int32 SlotsMax = 25;

   // Current
   UPROPERTY(Replicated, VisibleAnywhere, BlueprintReadOnly, Category = "Rade|Inventory")
      int32 WeightCurrent = 0;

   // Maximum weight actor can carry. In grams
   UPROPERTY(Replicated, EditDefaultsOnly, BlueprintReadOnly, Category = "Rade|Inventory")
      int32 WeightMax = 25000;

   //==========================================================================
   //                 Item list
   //==========================================================================

   UFUNCTION(BlueprintPure, Category = "Rade|Inventory")
      TArray<FRItemData> GetItems () const;

protected:

   // Network call when Item list has been updated
   UFUNCTION()
      virtual void OnRep_Items ();

   UPROPERTY(ReplicatedUsing = "OnRep_Items", Replicated)
      TArray<FRItemData> Items;

   virtual void CalcWeight ();

   // Used to calculate what changed in inventory
   UPROPERTY()
      TArray<FRItemData> LastItems;

public:

   // Item to be added upon game start
   UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Rade|Inventory")
      TArray<FRItemDataHandle> DefaultItems;

   // Delegate when Item list updated
   UPROPERTY(BlueprintAssignable, Category = "Rade|Inventory")
      FRInventoryEvent OnInventoryUpdated;

   // Delegate what Items have been added / removed
   UPROPERTY(BlueprintAssignable, Category = "Rade|Inventory")
      FRInventoryItemListEvent OnItemsChanged;

protected:
   UFUNCTION()
      void ReportInventoryUpdate ();

   UFUNCTION()
      void ReportInventoryUpdateDelayed ();

   UPROPERTY()
      bool ReportInventoryUpdateDelayedTriggered = false;

   //==========================================================================
   //                 Check if contains
   //==========================================================================
public:
   // --- Check if Inventory contains.
   //     Convenience functions

   UFUNCTION(BlueprintPure, Category = "Rade|Inventory")
      virtual bool HasItem_ID (const FString &ItemID, int32 Count) const;

   UFUNCTION(BlueprintPure, Category = "Rade|Inventory")
      virtual bool HasItem_Data (const FRItemData &ItemData) const;

      UFUNCTION(BlueprintPure, Category = "Rade|Inventory")
      virtual bool HasItem_Handle (const FRItemDataHandle &ItemHandle) const;

   UFUNCTION(BlueprintPure, Category = "Rade|Inventory")
      virtual bool HasItems_Handles (const TArray<FRItemDataHandle> &ItemHandles) const;

   //==========================================================================
   //                 Get item count
   //==========================================================================

   UFUNCTION(BlueprintPure, Category = "Rade|Inventory")
      virtual int GetItemCount_ID (const FString &ItemID) const;

   UFUNCTION(BlueprintPure, Category = "Rade|Inventory")
      virtual int GetItemCount_Data (const FRItemData &ItemData) const;

   UFUNCTION(BlueprintPure, Category = "Rade|Inventory")
      virtual int GetItemCount_Handle (const FRItemDataHandle &ItemHandle) const;

   //==========================================================================
   //                 Add to Inventory
   //==========================================================================

   UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category = "Rade|Inventory")
      virtual bool AddItem_Data (FRItemData ItemData);

   UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category = "Rade|Inventory")
      virtual bool AddItem_Handle (const FRItemDataHandle &ItemHandle);

   //==========================================================================
   //                 Remove from Inventory
   //==========================================================================

   // Remove item index
   UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category = "Rade|Inventory")
      virtual bool RemoveItem_Index (int32 ItemIdx, int32 Count = 1);

   // Remove item data
   UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category = "Rade|Inventory")
      virtual bool RemoveItem_Data (FRItemData ItemData);

      // Remove item arch
   UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category = "Rade|Inventory")
      virtual bool RemoveItem_Handle (const FRItemDataHandle &ItemHandle);

   //==========================================================================
   //                 Transfer between Inventories
   //==========================================================================

   // Transfer everything
   UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category = "Rade|Inventory")
      virtual bool TransferAll (URInventoryComponent *DstInventory);

   // Transfer a single item
   UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category = "Rade|Inventory")
      virtual bool TransferItem (URInventoryComponent *DstInventory,
                                 int32 SrcItemIdx,
                                 int32 SrcItemCount);

   //==========================================================================
   //                 Break Item
   //==========================================================================

   // Breaks item into components
   UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category = "Rade|Inventory")
      virtual bool BreakItem_Index (int32 ItemIdx,
                                    const UDataTable* BreakItemTable);

   UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category = "Rade|Inventory")
      virtual bool BreakItem_Data (const FRItemData &ItemData,
                                   const UDataTable* BreakItemTable);

   //==========================================================================
   //                 Craft Item
   //==========================================================================

   UFUNCTION(BlueprintCallable, Category = "Rade|Inventory")
      virtual bool CanCraftItem (const FDataTableRowHandle& CraftItem);

   UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category = "Rade|Inventory")
      virtual bool CraftItem (const FDataTableRowHandle &CraftItem);

   //==========================================================================
   //                 Use Item
   //==========================================================================

   UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category = "Rade|Inventory")
      virtual bool UseItem_Index (int32 ItemIdx);

   UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category = "Rade|Inventory")
      virtual bool UseItem_Data (const FRItemData &ItemData);

   //==========================================================================
   //                 Drop Item
   //==========================================================================

   // Pickup class will be created and items added to it's inventory
   UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Rade|Inventory")
      TSoftClassPtr<AActor> PickupClass;

   UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category = "Rade|Inventory")
      virtual bool DropItem_Index (int32 ItemIdx, int32 Count = 0);

   UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category = "Rade|Inventory")
      virtual bool DropItem_Data(const FRItemData &ItemData);

   //==========================================================================
   //                 Spawn Pickup
   //==========================================================================

   UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category = "Rade|Inventory")
      void SpawnPickup (const FRItemData &ItemData);

   //==========================================================================
   //                 Server versions of functions
   //==========================================================================
public:
   // --- Add Item
   UFUNCTION(Server, Reliable, BlueprintCallable, Category = "Rade|Inventory")
      void AddItem_Data_Server (
         URInventoryComponent *DstInventory, FRItemData ItemData) const;
      void AddItem_Data_Server_Implementation (
         URInventoryComponent *DstInventory, FRItemData ItemData) const;

   // --- Remove Item
   UFUNCTION(Server, Reliable, BlueprintCallable, Category = "Rade|Inventory")
      void RemoveItem_Index_Server (
         URInventoryComponent *SrcInventory, int32 ItemIdx, int32 Count = 1) const;
      void RemoveItem_Index_Server_Implementation (
         URInventoryComponent *SrcInventory, int32 ItemIdx, int32 Count = 1) const;

   UFUNCTION(Server, Reliable, BlueprintCallable, Category = "Rade|Inventory")
      void RemoveItem_Handle_Server (
         URInventoryComponent *SrcInventory, const FRItemDataHandle &ItemHandle) const;
      void RemoveItem_Handle_Server_Implementation (
         URInventoryComponent *SrcInventory, const FRItemDataHandle &ItemHandle) const;

   UFUNCTION(Server, Reliable, BlueprintCallable, Category = "Rade|Inventory")
      void RemoveItem_Data_Server (
         URInventoryComponent *SrcInventory, FRItemData ItemData) const;
      void RemoveItem_Data_Server_Implementation (
         URInventoryComponent *SrcInventory, FRItemData ItemData) const;

   // --- Transfer All
   UFUNCTION(Server, Reliable, BlueprintCallable, Category = "Rade|Inventory")
      void TransferAll_Server (
         URInventoryComponent *SrcInventory, URInventoryComponent *DstInventory) const;
      void TransferAll_Server_Implementation (
         URInventoryComponent *SrcInventory, URInventoryComponent *DstInventory) const;

   // --- Transfer Item
   UFUNCTION(Server, Reliable, BlueprintCallable, Category = "Rade|Inventory")
      void TransferItem_Server (
         URInventoryComponent *SrcInventory, URInventoryComponent *DstInventory,
         int32 SrcItemIdx, int32 SrcItemCount) const;
      void TransferItem_Server_Implementation (
         URInventoryComponent *SrcInventory, URInventoryComponent *DstInventory,
         int32 SrcItemIdx, int32 SrcItemCount) const;

   UFUNCTION(Server, Reliable, BlueprintCallable, Category = "Rade|Inventory")
      void UseItem_Index_Server (
         URInventoryComponent *SrcInventory, int32 ItemIdx) const;
      void UseItem_Index_Server_Implementation (
         URInventoryComponent *SrcInventory, int32 ItemIdx) const;

   UFUNCTION(Server, Reliable, BlueprintCallable, Category = "Rade|Inventory")
      void DropItem_Index_Server (
         URInventoryComponent *SrcInventory, int32 ItemIdx, int32 Count = 0) const;
      void DropItem_Index_Server_Implementation (
         URInventoryComponent *SrcInventory, int32 ItemIdx, int32 Count = 0) const;

   UFUNCTION(Server, Reliable, BlueprintCallable, Category = "Rade|Inventory")
      void BreakItem_Index_Server (
         URInventoryComponent *SrcInventory, int32 ItemIdx, const UDataTable* BreakItemTable) const;
      void BreakItem_Index_Server_Implementation (
         URInventoryComponent *SrcInventory, int32 ItemIdx, const UDataTable* BreakItemTable) const;

   UFUNCTION(Server, Reliable, BlueprintCallable, Category = "Rade|Inventory")
      void CraftItem_Server (
         URInventoryComponent *SrcInventory, const FDataTableRowHandle &CraftItem) const;
      void CraftItem_Server_Implementation (
         URInventoryComponent *SrcInventory, const FDataTableRowHandle &CraftItem) const;


   //==========================================================================
   //                 Save / Load
   //==========================================================================
public:
   // Status Saved / Loaded between sessions.
   UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Rade|Save")
      bool bSaveLoad = false;

	// Should be called during BeginPlay
	UFUNCTION()
      void ConnectToSaveMgr ();

protected:
   virtual void OnSave (FBufferArchive &SaveData) override;
   virtual void OnLoad (FMemoryReader  &LoadData) override;
};

