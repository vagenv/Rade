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

private:
      // Has authority to change inventory
      bool bIsServer = false;
public:

   //=============================================================================
   //                 Inventory info
   //=============================================================================

   // Maximum number
   UPROPERTY(Replicated, VisibleAnywhere, BlueprintReadWrite, Category = "Rade|Inventory")
      int32 SlotsMax = 5;

   // Current
   UPROPERTY(Replicated, VisibleAnywhere, BlueprintReadWrite, Category = "Rade|Inventory")
      float WeightCurrent = 0;

   // Maximum weight actor can carry
   UPROPERTY(Replicated, VisibleAnywhere, BlueprintReadWrite, Category = "Rade|Inventory")
      float WeightMax = 25;

   //=============================================================================
   //                 Item list
   //=============================================================================

   UFUNCTION(BlueprintPure, Category = "Rade|Inventory")
      TArray<FRItemData> GetItems () const;

protected:

   // // Network call when Item list has been updated
   // UFUNCTION()
   //    void OnRep_Items ();

   // UPROPERTY(ReplicatedUsing = "OnRep_Items", Replicated, VisibleAnywhere, BlueprintReadOnly, Category = "Rade|Inventory")
   UPROPERTY(Replicated)
      TArray<FRItemData> Items;


   void CalcWeight ();

public:

   // Item to be added upon game start
   UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Rade")
      TArray<FRDefaultItem> DefaultItems;

   // Delegate when Item list updated
   UPROPERTY(BlueprintAssignable, Category = "Rade|Inventory")
      FRInventoryEvent OnInventoryUpdated;

   //=============================================================================
   //                 Add/Remove to Inventory
   //=============================================================================

   UFUNCTION(BlueprintCallable, Category = "Rade|Inventory")
      bool AddItem        (FRItemData ItemData);
   UFUNCTION(BlueprintCallable, Category = "Rade|Inventory")
      bool AddItem_Arch   (const FRDefaultItem &Item);
   UFUNCTION(BlueprintCallable, Category = "Rade|Inventory")
      bool AddItem_Pickup (ARItemPickup *Pickup);
   UFUNCTION(BlueprintCallable, Category = "Rade|Inventory")
      bool RemoveItem     (int32 ItemIdx, int32 Count = 1);

   UFUNCTION(BlueprintCallable, Category = "Rade|Inventory")
      static bool TransferItem (URInventoryComponent *FromInventory,
                                URInventoryComponent *ToInventory,
                                int32 FromItemIdx,
                                int32 FromItemCount);

   //=============================================================================
   //                 Item use / drop events
   //=============================================================================

   // UFUNCTION(BlueprintCallable, Category = "Rade|Inventory")
   //    bool UseItem (int32 ItemIdx);

   // UFUNCTION(BlueprintCallable, Category = "Rade|Inventory")
   //    class ARItemPickup* DropItem (int32 ItemIdx, int32 Count = 1);

   //=============================================================================
   //                 Server
   //=============================================================================

protected:
   // --- In case the event occured on client, pass those events to server

   // UFUNCTION(Server, Reliable, BlueprintCallable, Category = "Rade|Inventory")
   //    void UseItem_Server                (int32 ItemIdx);
   //    void UseItem_Server_Implementation (int32 ItemIdx);

   // UFUNCTION(Server, Reliable, BlueprintCallable, Category = "Rade|Inventory")
   //    void DropItem_Server                (int32 ItemIdx, int32 Count = 1);
   //    void DropItem_Server_Implementation (int32 ItemIdx, int32 Count = 1);

   // UFUNCTION(Server, Reliable, BlueprintCallable, Category = "Rade|Inventory")
   //    void RemoveItem_Server                (int32 ItemIdx, int32 Count = 1);
   //    void RemoveItem_Server_Implementation (int32 ItemIdx, int32 Count = 1);

   // UFUNCTION(Server, Reliable, BlueprintCallable, Category = "Rade|Inventory")
   //    void AddItem_Server                (FRItemData ItemData);
   //    void AddItem_Server_Implementation (FRItemData ItemData);

   // UFUNCTION(Server, Reliable, BlueprintCallable, Category = "Rade|Inventory")
   //    void AddItem_Pickup_Server                (ARItemPickup *Pickup);
   //    void AddItem_Pickup_Server_Implementation (ARItemPickup *Pickup);


   // UFUNCTION(Server, Reliable, BlueprintCallable, Category = "Rade|Inventory")
   //    void TransferItem_Server                (URInventoryComponent *FromInventory,
   //                                             URInventoryComponent *ToInventory,
   //                                             int32 FromItemIdx,
   //                                             int32 FromItemCount);
   //    void TransferItem_Server_Implementation (URInventoryComponent *FromInventory,
   //                                             URInventoryComponent *ToInventory,
   //                                             int32 FromItemIdx,
   //                                             int32 FromItemCount);

   //=============================================================================
   //                 Events
   //=============================================================================

   // UFUNCTION(BlueprintImplementableEvent, Category = "Rade|Item")
   //    void BP_Used (URItem *Item);

   // UFUNCTION(BlueprintImplementableEvent, Category = "Rade|Item")
   //    void BP_Droped (URItem *Item, ARItemPickup *Pickup);


   //=============================================================================
   //                 Pickups
   //=============================================================================

protected:
   // List of currently available pickup
   // UPROPERTY(ReplicatedUsing = "OnRep_CurrentPickups", Replicated, VisibleAnywhere, BlueprintReadOnly, Category = "Rade|Inventory",
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
   UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Rade|Inventory")
      bool bCheckClosestPickup = false;

   UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Rade|Inventory")
      float CheckClosestDelay = 0.5f;

   //=============================================================================
   //                 Save / Load
   //=============================================================================

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

