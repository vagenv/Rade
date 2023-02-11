// Copyright 2015-2023 Vagen Ayrapetyan

#include "REquipmentMgrComponent.h"
#include "RUtilLib/RLog.h"
#include "REquipmentTypes.h"
#include "REquipmentSlotComponent.h"
#include "RInventoryLib/RItemAction.h"
#include "RStatusLib/RStatusMgrComponent.h"

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

   bool success = Equip (ItemData);

   // --- If Custom Action is defined
   if (success && ItemData.Action) {
      URItemAction *ItemBP = ItemData.Action->GetDefaultObject<URItemAction>();
      if (ItemBP) {
         ItemBP->Used (this, ItemData, ItemIdx);
         BP_Used (ItemIdx);
      }
   }
   return success;
}

bool UREquipmentMgrComponent::Equip (const FREquipmentData &EquipmentData)
{
   if (!EquipmentData.EquipmentSlot.Get ()) {
      R_LOG_PRINTF ("Equipment item [%s] doesn't have a valid equip slot set.", *EquipmentData.Name);
      return false;
   }

   UREquipmentSlotComponent *EquipmentSlot = GetEquipmentSlot (EquipmentData.EquipmentSlot);
   if (!EquipmentSlot) {
      R_LOG_PRINTF ("Equipment item [%s] required slot [%s] not found on character",
         *EquipmentData.Name, *EquipmentData.EquipmentSlot->GetName ());
      return false;
   }

   URStatusMgrComponent* StatusMgr = GetStatusMgr ();
   if (!EquipmentData.RequiredStats.Empty ()) {
      if (!StatusMgr) {
         R_LOG_PRINTF ("Equipment item [%s] failed. URStatusMgrComponent not found", *EquipmentData.Name);
         return false;
      }
      if (!StatusMgr->HasStats (EquipmentData.RequiredStats)) return false;
   }

   // --- Unequip if busy
   if (EquipmentSlot->Busy) {

      // If equip has been called on same item -> Only unequip.
      if (EquipmentSlot->EquipmentData.Name == EquipmentData.Name) {
         return UnEquip (EquipmentSlot);
      }

      if (!UnEquip (EquipmentSlot)) return false;
   }

   // --- Add Stats and Effects
   if (StatusMgr) {
      StatusMgr->AddStat (EquipmentData.Stats);
      StatusMgr->AddResistance (EquipmentData.Resistence);
      // TODO: Add addition stats and effects
   }

   // --- Update slot data
   EquipmentSlot->EquipmentData = EquipmentData;
   EquipmentSlot->Busy = true;
   EquipmentSlot->OnSlotUpdated.Broadcast ();
   OnEquipmentUpdated.Broadcast ();
   return true;
}

bool UREquipmentMgrComponent::UnEquip (UREquipmentSlotComponent *EquipmentSlot)
{
   if (!EquipmentSlot)       return false;
   if (!EquipmentSlot->Busy) return false;

   URStatusMgrComponent* StatusMgr = GetStatusMgr ();
   if (StatusMgr) {
      StatusMgr->RmStat (EquipmentSlot->EquipmentData.Stats);
      StatusMgr->RmResistance (EquipmentSlot->EquipmentData.Resistence);
      // TODO: Remove effects
   }
   EquipmentSlot->Busy = false;
   EquipmentSlot->EquipmentData = FREquipmentData();
   EquipmentSlot->OnSlotUpdated.Broadcast ();
   OnEquipmentUpdated.Broadcast ();
   return true;
}

// --- Get Components

URStatusMgrComponent* UREquipmentMgrComponent::GetStatusMgr () const
{
   TArray<URStatusMgrComponent*> StatusMgrList;
   GetOwner ()->GetComponents (StatusMgrList);
   if (StatusMgrList.Num ()) return StatusMgrList[0];
   else                      return nullptr;
}

UREquipmentSlotComponent * UREquipmentMgrComponent::GetEquipmentSlot (const TSubclassOf<UREquipmentSlotComponent> &SlotClass) const
{
   TArray<UREquipmentSlotComponent*> CurrentEquipmentSlots;
   GetOwner ()->GetComponents (CurrentEquipmentSlots);

   UREquipmentSlotComponent *EquipmentSlot = nullptr;

   // --- Find equip slot
   for (auto &itSlot : CurrentEquipmentSlots) {
      if (itSlot->GetClass () == SlotClass) {

         // First slot available
         if (!EquipmentSlot) EquipmentSlot = itSlot;

         // Empty Slot found
         if (!itSlot->Busy) {
            EquipmentSlot = itSlot;
            break;
         }
      }
   }
   return EquipmentSlot;
}

