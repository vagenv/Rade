// Copyright 2015-2023 Vagen Ayrapetyan

#include "REquipmentMgrComponent.h"
#include "RUtilLib/Rlog.h"
#include "REquipmentTypes.h"
#include "RInventoryLib/RItemAction.h"

//=============================================================================
//                 Core
//=============================================================================

UREquipmentMgrComponent::UREquipmentMgrComponent ()
{
   SetIsReplicatedByDefault (true);
}

// Replication
void UREquipmentMgrComponent::GetLifetimeReplicatedProps (TArray<FLifetimeProperty> &OutLifetimeProps) const
{
   Super::GetLifetimeReplicatedProps (OutLifetimeProps);
}

void UREquipmentMgrComponent::BeginPlay()
{
   Super::BeginPlay();

   // --- Create Equipment Slots
   EquipmentSlotsRuntime.Empty ();
   for (auto const& SlotInfo : EquipmentSlots) {
      TObjectPtr<UREquipmentSlot> NewSlot = NewObject<UREquipmentSlot> (this, SlotInfo);
      NewSlot->Owner = GetOwner ();
      NewSlot->EquipmentMgr = this;
      NewSlot->World = GetWorld ();
      EquipmentSlotsRuntime.Add (NewSlot);
   }
}

void UREquipmentMgrComponent::EndPlay (const EEndPlayReason::Type EndPlayReason)
{
   Super::EndPlay (EndPlayReason);
}

bool UREquipmentMgrComponent::UseItem (int32 ItemIdx)
{
   if (!bIsServer) {
      R_LOG ("Client has no authority to perform this action.");
      return false;
   }

   // Valid index
   if (!Items.IsValidIndex (ItemIdx)) {
      R_LOG_PRINTF ("Invalid Item Index [%d]. Must be [0-%d]",
         ItemIdx, Items.Num ());
      return false;
   }

   // --- Not an equipment item. Pass management to Inventory.
   FREquipmentData ItemData;
   if (!FREquipmentData::Cast (Items[ItemIdx], ItemData)) {
      return Super::UseItem (ItemIdx);
   }

   if (!ItemData.EquipmentSlot.Get ()) {
      R_LOG_PRINTF ("Equipment item [%s] doesn't have a valid equip slot set.", *ItemData.Name);
      return false;
   }

   UREquipmentSlot *SupportedSlot = nullptr;

   // --- Find equip slot
   for (auto &EquipmentSlot : EquipmentSlotsRuntime) {
      if (EquipmentSlot->GetClass () == ItemData.EquipmentSlot) {

         // First slot available
         if (!SupportedSlot) SupportedSlot = EquipmentSlot;

         // Empty Slot found
         if (!EquipmentSlot->Busy) {
            SupportedSlot = EquipmentSlot;
            break;
         }
      }
   }

   if (!SupportedSlot) {
      R_LOG_PRINTF ("Equipment item [%s] required slot [%s] not found on character",
         *ItemData.Name, *ItemData.EquipmentSlot->GetName ());
      return false;
   }


   // Equip
   R_LOG_PRINTF ("Equiping item [%s] to slot [%s].", *ItemData.Name, *SupportedSlot->GetPathName ());

   // TODO: Check equip required stats

   if (SupportedSlot->Busy) {
      // TODO: Remove stats and effects
      SupportedSlot->Busy = false;
      SupportedSlot->Updated ();
   }

   // TODO: Replace with actual unequip
   if (SupportedSlot->EquipmentData.Name == ItemData.Name) {
      SupportedSlot->EquipmentData = FREquipmentData();
      return true;
   }

   // --- Update slot data
   SupportedSlot->EquipmentData = ItemData;
   SupportedSlot->Busy = true;
   SupportedSlot->Updated ();

   // TODO: Add addition stats and effects

   // --- Equipment Action if defined
   if (ItemData.Action) {
      URItemAction *ItemBP = ItemData.Action->GetDefaultObject<URItemAction>();
      if (ItemBP) {
         ItemBP->Used (this, ItemData, ItemIdx);
         BP_Used (ItemIdx);
      }
   }

   return true;
}

