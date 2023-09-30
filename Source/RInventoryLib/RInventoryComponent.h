// Copyright 2015-2023 Vagen Ayrapetyan

#pragma once

#include "RItemTypes.h"
#include "Components/ActorComponent.h"
#include "RSaveLib/RSaveInterface.h"
#include "RInventoryComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE (FRInventoryEvent);

class ARItemPickup;
struct FStreamableHandle;

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

public:

   // Item to be added upon game start
   UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Rade|Inventory")
      TArray<FRItemDataHandle> DefaultItems;

   // Delegate when Item list updated
   UPROPERTY(BlueprintAssignable, Category = "Rade|Inventory")
      FRInventoryEvent OnInventoryUpdated;

protected:
   UFUNCTION()
      void ReportInventoryUpdate () const;

   //==========================================================================
   //                 Check if contains
   //==========================================================================
public:
   // --- Check if Inventory contains.
   //     Convenience functions

   UFUNCTION(BlueprintPure, Category = "Rade|Inventory")
      virtual bool HasItem_Arch (const FRItemDataHandle &CheckItem) const;

   UFUNCTION(BlueprintPure, Category = "Rade|Inventory")
      virtual bool HasItem_Item (FRItemData CheckItem) const;

   UFUNCTION(BlueprintPure, Category = "Rade|Inventory")
      virtual bool HasItems_Handles (const TArray<FRItemDataHandle> &CheckItems) const;

   //==========================================================================
   //                 Get item count
   //==========================================================================

   UFUNCTION(BlueprintPure, Category = "Rade|Inventory")
      virtual int GetItemCount_ID (const FString &ID) const;

   UFUNCTION(BlueprintPure, Category = "Rade|Inventory")
      virtual int GetItemCount_Item (const FRItemData &CheckItem) const;

   //==========================================================================
   //                 Add to Inventory
   //==========================================================================

   // Add Default item
   UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category = "Rade|Inventory")
      virtual bool AddItem_Arch (const FRItemDataHandle &Item);

   // Add Item struct
   UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category = "Rade|Inventory")
      virtual bool AddItem_Item (FRItemData ItemData);

   //==========================================================================
   //                 Remove from Inventory
   //==========================================================================

   // Remove item index
   UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category = "Rade|Inventory")
      virtual bool RemoveItem_Index (int32 ItemIdx, int32 Count = 1);

   // Remove item arch
   UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category = "Rade|Inventory")
      virtual bool RemoveItem_Arch  (const FRItemDataHandle &ItemHandle);

   // Remove item data
   UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category = "Rade|Inventory")
      virtual bool RemoveItem_Data  (FRItemData ItemData);

   //==========================================================================
   //                 Transfer between Inventories
   //==========================================================================

   // Transfer everything
   UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category = "Rade|Inventory")
      virtual bool TransferAll  (URInventoryComponent *DstInventory);

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
      virtual bool BreakItem_Index (int32 ItemIdx, UDataTable* BreakItemTable);

   UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category = "Rade|Inventory")
      virtual bool BreakItem_Item (const FRItemData &ItemData, UDataTable* BreakItemTable);

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
      virtual bool UseItem_Index  (int32 ItemIdx);

   UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category = "Rade|Inventory")
      virtual bool UseItem_Item  (const FRItemData &ItemData);

   //==========================================================================
   //                 Drop Item
   //==========================================================================

   UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category = "Rade|Inventory")
      virtual bool DropItem_Index (int32 ItemIdx, int32 Count = 0);

   UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category = "Rade|Inventory")
      virtual bool DropItem_ItemData (const FRItemData &ItemData);

   //==========================================================================
   //                 Util
   //==========================================================================

private:
   // Handle to async load task
   TSharedPtr<FStreamableHandle> PickupLoadHandle;

   // Wrapper function
   ARItemPickup* SpawnPickup (TSubclassOf<ARItemPickup> PickupClass, FRItemData ItemData);

   //==========================================================================
   //                 Server versions of functions
   //==========================================================================
public:
   // --- Add Item
   UFUNCTION(Server, Reliable, BlueprintCallable, Category = "Rade|Inventory")
      void AddItem_Server (
         URInventoryComponent *DstInventory, FRItemData ItemData) const;
      void AddItem_Server_Implementation (
         URInventoryComponent *DstInventory, FRItemData ItemData) const;

   // --- Remove Item
   UFUNCTION(Server, Reliable, BlueprintCallable, Category = "Rade|Inventory")
      void RemoveItem_Index_Server (
         URInventoryComponent *SrcInventory, int32 ItemIdx, int32 Count = 1) const;
      void RemoveItem_Index_Server_Implementation (
         URInventoryComponent *SrcInventory, int32 ItemIdx, int32 Count = 1) const;

   UFUNCTION(Server, Reliable, BlueprintCallable, Category = "Rade|Inventory")
      void RemoveItem_Arch_Server (
         URInventoryComponent *SrcInventory, const FRItemDataHandle &ItemHandle) const;
      void RemoveItem_Arch_Server_Implementation (
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
      void UseItem_Server (
         URInventoryComponent *SrcInventory, int32 ItemIdx) const;
      void UseItem_Server_Implementation (
         URInventoryComponent *SrcInventory, int32 ItemIdx) const;

   UFUNCTION(Server, Reliable, BlueprintCallable, Category = "Rade|Inventory")
      void DropItem_Server (
         URInventoryComponent *SrcInventory, int32 ItemIdx, int32 Count = 0) const;
      void DropItem_Server_Implementation (
         URInventoryComponent *SrcInventory, int32 ItemIdx, int32 Count = 0) const;

   UFUNCTION(Server, Reliable, BlueprintCallable, Category = "Rade|Inventory")
      void BreakItem_Server (
         URInventoryComponent *SrcInventory, int32 ItemIdx, UDataTable* BreakItemTable) const;
      void BreakItem_Server_Implementation (
         URInventoryComponent *SrcInventory, int32 ItemIdx, UDataTable* BreakItemTable) const;

   UFUNCTION(Server, Reliable, BlueprintCallable, Category = "Rade|Inventory")
      void CraftItem_Server (
         URInventoryComponent *SrcInventory, const FDataTableRowHandle &CraftItem) const;
      void CraftItem_Server_Implementation (
         URInventoryComponent *SrcInventory, const FDataTableRowHandle &CraftItem) const;

   //==========================================================================
   //                 Save / Load
   //==========================================================================

public:
   UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Rade|Inventory")
      bool bSaveLoad = false;

protected:
   virtual void OnSave (FBufferArchive &SaveData) override;
   virtual void OnLoad (FMemoryReader &LoadData) override;
};

