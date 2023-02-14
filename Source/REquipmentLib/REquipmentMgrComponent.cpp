// Copyright 2015-2023 Vagen Ayrapetyan

#include "REquipmentMgrComponent.h"
#include "RUtilLib/RLog.h"
#include "REquipmentTypes.h"
#include "REquipmentSlotComponent.h"
#include "RInventoryLib/RItemAction.h"
#include "RStatusLib/RStatusMgrComponent.h"
#include "RUtilLib/RCheck.h"

//=============================================================================
//                 Core
//=============================================================================

UREquipmentMgrComponent::UREquipmentMgrComponent ()
{
   SetIsReplicatedByDefault (true);

   // Equip Load
   FRichCurve* StrToWeightMaxData = StrToWeightMax.GetRichCurve ();
   StrToWeightMaxData->AddKey (   0,   10); // Minimum
   StrToWeightMaxData->AddKey (   1, 10.5);
   StrToWeightMaxData->AddKey (  10,   15);
   StrToWeightMaxData->AddKey ( 100,   50);
   StrToWeightMaxData->AddKey (1000,  100);
   StrToWeightMaxData->AddKey (5000,  150);

   // Weight To Evasion
   FRichCurve* WeightToEvasionData = WeightToEvasion.GetRichCurve ();
   WeightToEvasionData->AddKey ( 40, 100); // Minimum
   WeightToEvasionData->AddKey ( 50,  95);
   WeightToEvasionData->AddKey ( 60,  90);
   WeightToEvasionData->AddKey ( 70,  80);
   WeightToEvasionData->AddKey ( 80,  70);
   WeightToEvasionData->AddKey ( 90,  55);
   WeightToEvasionData->AddKey (100,  40);

   // Weight to Move speed
   FRichCurve* WeightToMoveSpeedData = WeightToMoveSpeed.GetRichCurve ();
   WeightToMoveSpeedData->AddKey ( 40, 100); // Minimum
   WeightToMoveSpeedData->AddKey ( 50,  95);
   WeightToMoveSpeedData->AddKey ( 60,  90);
   WeightToMoveSpeedData->AddKey ( 70,  85);
   WeightToMoveSpeedData->AddKey ( 80,  80);
   WeightToMoveSpeedData->AddKey ( 90,  70);
   WeightToMoveSpeedData->AddKey (100,  60);
}

// Replication
void UREquipmentMgrComponent::GetLifetimeReplicatedProps (TArray<FLifetimeProperty> &OutLifetimeProps) const
{
   Super::GetLifetimeReplicatedProps (OutLifetimeProps);
}

void UREquipmentMgrComponent::BeginPlay ()
{
   Super::BeginPlay ();

   if (R_IS_NET_ADMIN) {
      if (URStatusMgrComponent *StatusMgr = GetStatusMgr ()) {
         StatusMgr->OnStatusUpdated.AddDynamic (this, &UREquipmentMgrComponent::OnStatusUpdated);
      }
      OnStatusUpdated ();
   }
}

void UREquipmentMgrComponent::EndPlay (const EEndPlayReason::Type EndPlayReason)
{
   Super::EndPlay (EndPlayReason);
}

void UREquipmentMgrComponent::CalcWeight ()
{
   Super::CalcWeight ();
   const FRichCurve* WeightToEvasionData   = WeightToEvasion.GetRichCurveConst ();
   const FRichCurve* WeightToMoveSpeedData = WeightToMoveSpeed.GetRichCurveConst ();

   if (!ensure (WeightToEvasionData))    return;
   if (!ensure (WeightToMoveSpeedData))  return;

   float EquipLoad = WeightCurrent * 100. / WeightMax;
   float EquipEvasionFactor = WeightToEvasionData->Eval (EquipLoad);

   // TODO : Add Effect to Player

   //EvasionChance *= (EquipEvasionFactor / 100.);
   //MoveSpeedFactor = EquipToMoveSpeedData->Eval (EquipLoad) / 100.;
}

void UREquipmentMgrComponent::OnStatusUpdated ()
{
   const FRichCurve* StrToWeightMaxData = StrToWeightMax.GetRichCurveConst ();
   if (!ensure (StrToWeightMaxData)) return;

   URStatusMgrComponent *StatusMgr = GetStatusMgr ();
   if (StatusMgr) {
      FRCharacterStats StatsTotal = StatusMgr->GetStatsTotal ();
      WeightMax = StrToWeightMaxData->Eval (StatsTotal.Strength);
   }
}

bool UREquipmentMgrComponent::UseItem (int32 ItemIdx)
{
   R_RETURN_IF_NOT_ADMIN_BOOL;

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
   R_RETURN_IF_NOT_ADMIN_BOOL;
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
   return Equip (EquipmentSlot, EquipmentData);
}

bool UREquipmentMgrComponent::Equip (UREquipmentSlotComponent *EquipmentSlot, const FREquipmentData &EquipmentData)
{
   R_RETURN_IF_NOT_ADMIN_BOOL;
   if (!EquipmentSlot) {
      R_LOG ("Invalid Equipment Slot pointer");
      return false;
   }
   if (!EquipmentData.EquipmentSlot.Get ()) {
      R_LOG_PRINTF ("Equipment item [%s] doesn't have a valid equip slot set.", *EquipmentData.Name);
      return false;
   }
   if (EquipmentSlot->GetClass () != EquipmentData.EquipmentSlot) {
      R_LOG_PRINTF ("Inconsistent Slot Class: Item [%s] Slot []",
         *EquipmentSlot->GetClass ()->GetName (),
         *EquipmentData.EquipmentSlot->GetName ()
         );
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
      StatusMgr->SetExtraStat (EquipmentData.Name, EquipmentData.Stats);
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
   R_RETURN_IF_NOT_ADMIN_BOOL;
   if (!EquipmentSlot)       return false;
   if (!EquipmentSlot->Busy) return false;

   URStatusMgrComponent* StatusMgr = GetStatusMgr ();
   if (StatusMgr) {
      StatusMgr->RmExtraStat (EquipmentSlot->EquipmentData.Name);
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

UREquipmentSlotComponent* UREquipmentMgrComponent::GetEquipmentSlot (const TSubclassOf<UREquipmentSlotComponent> &SlotClass) const
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

