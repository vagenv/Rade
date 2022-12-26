// Copyright 2015-2023 Vagen Ayrapetyan
#pragma once

#include "RItemTypes.h"
#include "Components/ActorComponent.h"
#include "RInventoryComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE (FRInventoryEvent);

class URItem;
class ARItemPickup;
class URInventoryComponent;

// Inventory Component. Holds all items an actor own
UCLASS(Blueprintable)
class RINVENTORYLIB_API URInventoryComponent : public UActorComponent
{
   GENERATED_BODY()
public:

   // Base events
   URInventoryComponent ();
   virtual void GetLifetimeReplicatedProps (TArray<FLifetimeProperty> &OutLifetimeProps) const override;
   virtual void BeginPlay () override;

private:
      // Has authority to change inventory
      bool bIsServer = false;
public:

   // -------------------------------------------------------------------------
   //                 Inventory info
   // -------------------------------------------------------------------------

   // Number of item slots
   UPROPERTY(Replicated, EditDefaultsOnly, Category = "Rade|Inventory")
      int32 Capacity;

   // Total Weight of Inventory
   UPROPERTY(Replicated, VisibleAnywhere, BlueprintReadOnly, Category = "Rade|Inventory")
      float TotalWeight;


   // -------------------------------------------------------------------------
   //                 Item list
   // -------------------------------------------------------------------------
   UFUNCTION(BlueprintPure, Category = "Rade|Inventory")
      const TArray<FRItemData> GetItems() const;
protected:
   UPROPERTY(ReplicatedUsing = "OnRep_Items", Replicated, VisibleAnywhere, BlueprintReadOnly, Category = "Rade|Inventory")
      TArray<FRItemData> Items;

   // Network call when Item list has been updated
   UFUNCTION()
      void OnRep_Items ();

public:

   // Item to be added upon game start
   UPROPERTY(EditDefaultsOnly, Category = "Rade|Inventory")
      TArray<TSubclassOf<URItem> > DefaultItems;

   // Delegate when Item list updated
   UPROPERTY(BlueprintAssignable, Category = "Rade|Inventory")
      FRInventoryEvent OnInventoryUpdated;


   // -------------------------------------------------------------------------
   //                 Pickups
   // -------------------------------------------------------------------------


   // List of currently available pickup
   UPROPERTY(ReplicatedUsing = "OnRep_CurrentPickups", Replicated, VisibleAnywhere, BlueprintReadOnly, Category = "Rade|Inventory",
             meta = (GetByRef))
      TArray<ARItemPickup*> CurrentPickups;

   // Network call when Item list has been updated
   UFUNCTION()
      void OnRep_CurrentPickups ();

   UFUNCTION(BlueprintPure, Category = "Rade|Inventory")
      ARItemPickup* GetClosestPickup ();

   // Delegate when pickup list updated
   UPROPERTY(BlueprintAssignable, Category = "Rade|Inventory")
      FRInventoryEvent OnPickupsUpdated;

   // -------------------------------------------------------------------------
   //                 Add/Remove to Inventory
   // -------------------------------------------------------------------------

   UFUNCTION(BlueprintCallable, Category = "Rade|Inventory")
      bool AddItem        (FRItemData ItemData);
   UFUNCTION(BlueprintCallable, Category = "Rade|Inventory")
      bool AddItem_Arch   (TSubclassOf<URItem> Item);
   UFUNCTION(BlueprintCallable, Category = "Rade|Inventory")
      bool AddItem_Pickup (ARItemPickup *Pickup);
   UFUNCTION(BlueprintCallable, Category = "Rade|Inventory")
      bool RemoveItem (int32 ItemIdx, int32 Count = 1);

   UFUNCTION(BlueprintCallable, Category = "Rade|Inventory")
   static bool TransferItem (URInventoryComponent *FromInventory,
                             URInventoryComponent *ToInventory,
                             int32 FromItemIdx,
                             int32 FromItemCount);

   // -------------------------------------------------------------------------
   //                 Item use / drop events
   // -------------------------------------------------------------------------

   UFUNCTION(BlueprintCallable, Category = "Rade|Inventory")
      bool UseItem (int32 ItemIdx);

   UFUNCTION(BlueprintCallable, Category = "Rade|Inventory")
      class ARItemPickup* DropItem (int32 ItemIdx, int32 Count = 1);

   // -------------------------------------------------------------------------
   //                 Server
   // -------------------------------------------------------------------------

protected:
   // --- In case the event occured on client, pass those events to server

   UFUNCTION(Server, Reliable, BlueprintCallable, Category = "Rade|Inventory")
      void UseItem_Server                (int32 ItemIdx);
      void UseItem_Server_Implementation (int32 ItemIdx);

   UFUNCTION(Server, Reliable, BlueprintCallable, Category = "Rade|Inventory")
      void DropItem_Server                (int32 ItemIdx, int32 Count = 1);
      void DropItem_Server_Implementation (int32 ItemIdx, int32 Count = 1);

   UFUNCTION(Server, Reliable, BlueprintCallable, Category = "Rade|Inventory")
      void RemoveItem_Server                (int32 ItemIdx, int32 Count = 1);
      void RemoveItem_Server_Implementation (int32 ItemIdx, int32 Count = 1);

   UFUNCTION(Server, Reliable, BlueprintCallable, Category = "Rade|Inventory")
      void AddItem_Server                (FRItemData ItemData);
      void AddItem_Server_Implementation (FRItemData ItemData);

   UFUNCTION(Server, Reliable, BlueprintCallable, Category = "Rade|Inventory")
      void AddItem_Pickup_Server                (ARItemPickup *Pickup);
      void AddItem_Pickup_Server_Implementation (ARItemPickup *Pickup);


   UFUNCTION(Server, Reliable, BlueprintCallable, Category = "Rade|Inventory")
      void TransferItem_Server                (URInventoryComponent *FromInventory,
                                               URInventoryComponent *ToInventory,
                                               int32 FromItemIdx,
                                               int32 FromItemCount);
      void TransferItem_Server_Implementation (URInventoryComponent *FromInventory,
                                               URInventoryComponent *ToInventory,
                                               int32 FromItemIdx,
                                               int32 FromItemCount);

   // -------------------------------------------------------------------------
   //                 Events
   // -------------------------------------------------------------------------

   UFUNCTION(BlueprintImplementableEvent, Category = "Rade|Item")
      void BP_Used (URItem *Item);

   UFUNCTION(BlueprintImplementableEvent, Category = "Rade|Item")
      void BP_Droped (URItem *Item, ARItemPickup *Pickup);


   // -------------------------------------------------------------------------
   //                 Save / Load
   // -------------------------------------------------------------------------

public:
   // Inventory Saved / Loaded between sessions.
   // Should be used only for Player.
   // Careful with collision of 'InventoryUniqueId'
   UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Rade|Inventory")
      bool bSaveLoadInventory = false;

protected:
   // Rade Save events
   UFUNCTION()
      void OnSave ();
   UFUNCTION()
      void OnLoad ();
};
