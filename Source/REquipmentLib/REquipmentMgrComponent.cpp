// Copyright 2015-2023 Vagen Ayrapetyan

#include "REquipmentMgrComponent.h"
#include "REquipmentTypes.h"
#include "REquipmentSlotComponent.h"

#include "RUtilLib/RUtil.h"
#include "RUtilLib/RLog.h"
#include "RUtilLib/RCheck.h"
#include "RUtilLib/RJson.h"

#include "RStatusLib/RStatusMgrComponent.h"
#include "RInventoryLib/RItemAction.h"

//=============================================================================
//                 Core
//=============================================================================

UREquipmentMgrComponent::UREquipmentMgrComponent ()
{
   SetIsReplicatedByDefault (true);

   // Equip Load
   FRichCurve* StrToWeightMaxData = StrToWeightMax.GetRichCurve ();
   StrToWeightMaxData->AddKey (   0,   10000); // Minimum
   StrToWeightMaxData->AddKey (   1,   10050);
   StrToWeightMaxData->AddKey (  10,   15000);
   StrToWeightMaxData->AddKey ( 100,   50000);
   StrToWeightMaxData->AddKey (1000,  100000);
   StrToWeightMaxData->AddKey (5000,  150000);

   // Weight percent To Evasion
   FRichCurve* WeightToEvasionData = WeightToEvasion.GetRichCurve ();
   WeightToEvasionData->AddKey ( 40,   0); // Minimum
   WeightToEvasionData->AddKey ( 50,  -5);
   WeightToEvasionData->AddKey ( 60, -10);
   WeightToEvasionData->AddKey ( 70, -20);
   WeightToEvasionData->AddKey ( 80, -30);
   WeightToEvasionData->AddKey ( 90, -45);
   WeightToEvasionData->AddKey (100, -60);

   // Weight to Move speed
   FRichCurve* WeightToMoveSpeedData = WeightToMoveSpeed.GetRichCurve ();
   WeightToMoveSpeedData->AddKey ( 40,   0); // Minimum
   WeightToMoveSpeedData->AddKey ( 50,  -5);
   WeightToMoveSpeedData->AddKey ( 60, -10);
   WeightToMoveSpeedData->AddKey ( 70, -15);
   WeightToMoveSpeedData->AddKey ( 80, -20);
   WeightToMoveSpeedData->AddKey ( 90, -30);
   WeightToMoveSpeedData->AddKey (100, -40);
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
      if (URStatusMgrComponent *StatusMgr = URUtil::GetComponent<URStatusMgrComponent> (GetOwner ())) {
         StatusMgr->OnStatusUpdated.AddDynamic (this, &UREquipmentMgrComponent::OnStatusUpdated);
      }
      OnStatusUpdated ();
   }
}

void UREquipmentMgrComponent::EndPlay (const EEndPlayReason::Type EndPlayReason)
{
   Super::EndPlay (EndPlayReason);
}

//=============================================================================
//           Weight penalty using Inventory and StatusMgr
//=============================================================================

void UREquipmentMgrComponent::CalcWeight ()
{
   R_RETURN_IF_NOT_ADMIN;
   Super::CalcWeight ();

   URStatusMgrComponent *StatusMgr = URUtil::GetComponent<URStatusMgrComponent> (GetOwner ());
   if (!StatusMgr) return;

   const FRichCurve* WeightToEvasionData   = WeightToEvasion.GetRichCurveConst ();
   const FRichCurve* WeightToMoveSpeedData = WeightToMoveSpeed.GetRichCurveConst ();

   if (!ensure (WeightToEvasionData))    return;
   if (!ensure (WeightToMoveSpeedData))  return;

   float EquipLoad = WeightCurrent * 100. / WeightMax;
   FRPassiveStatusEffect EvasionEffect;
   EvasionEffect.Scale  = ERStatusEffectScale::PERCENT;
   EvasionEffect.Target = ERStatusEffectTarget::Evasion;
   EvasionEffect.Value  = WeightToEvasionData->Eval (EquipLoad);

   FRPassiveStatusEffect MoveSpeedEffect;
   MoveSpeedEffect.Scale  = ERStatusEffectScale::PERCENT;
   MoveSpeedEffect.Target = ERStatusEffectTarget::MoveSpeed;
   MoveSpeedEffect.Value  = WeightToMoveSpeedData->Eval (EquipLoad);

   TArray<FRPassiveStatusEffect> Effects;
   Effects.Add (EvasionEffect);
   Effects.Add (MoveSpeedEffect);

   StatusMgr->SetPassiveEffects ("Weight Penalty", Effects);
}

void UREquipmentMgrComponent::OnStatusUpdated ()
{
   const FRichCurve* StrToWeightMaxData = StrToWeightMax.GetRichCurveConst ();
   if (!ensure (StrToWeightMaxData)) return;

   URStatusMgrComponent *StatusMgr = URUtil::GetComponent<URStatusMgrComponent> (GetOwner ());
   if (StatusMgr) {
      FRCoreStats StatsTotal = StatusMgr->GetCoreStats_Total ();
      WeightMax = StrToWeightMaxData->Eval (StatsTotal.STR);
      if (LastWeightMax != WeightMax) {
         LastWeightMax = WeightMax;
         CalcWeight ();
      }
   }
}

//=============================================================================
//                 Use/Drop override
//=============================================================================

bool UREquipmentMgrComponent::UseItem (int32 ItemIdx)
{
   R_RETURN_IF_NOT_ADMIN_BOOL;

   // Valid index
   if (!Items.IsValidIndex (ItemIdx)) {
      R_LOG_PRINTF ("Invalid Item Index [%d]. Must be [0-%d]",
         ItemIdx, Items.Num ());
      return false;
   }

   {
      FRConsumableItemData ItemData;
      if (FRConsumableItemData::Cast (Items[ItemIdx], ItemData)) {
         if (!ItemData.Used (GetOwner (), this)) return false;
         BP_Used (ItemIdx);
         if (ItemData.DestroyOnAction) return RemoveItem_Index (ItemIdx, 1);
         return true;
      }
   }

   // --- Not an equipment item. Pass management to Inventory.
   FREquipmentData ItemData;
   if (!FREquipmentData::Cast (Items[ItemIdx], ItemData)) {
      return Super::UseItem (ItemIdx);
   }

   bool success = Equip (ItemData);

   // --- If Custom Action is defined
   if (success && ItemData.Action) {
      if (!ItemData.Used (GetOwner (), this)) return false;
      BP_Used (ItemIdx);
   }
   return success;
}

ARItemPickup* UREquipmentMgrComponent::DropItem (int32 ItemIdx, int32 Count)
{
   R_RETURN_IF_NOT_ADMIN_NULL;


   // --- Check if Item should be unequiped

   FREquipmentData ItemData;
   // Is Equipment item
   if (FREquipmentData::Cast (Items[ItemIdx], ItemData)) {

      // Get target slot type
      UREquipmentSlotComponent *EquipmentSlot = GetEquipmentSlot (ItemData.EquipmentSlot);

      if (EquipmentSlot) {

         // Item equiped
         if (EquipmentSlot->EquipmentData.Name == ItemData.Name) {
            UnEquip (EquipmentSlot);
         }
      }
   }

   return Super::DropItem (ItemIdx, Count);
}

//=============================================================================
//                 Equip/Unequip
//=============================================================================

bool UREquipmentMgrComponent::Equip (const FREquipmentData &EquipmentData)
{
   R_RETURN_IF_NOT_ADMIN_BOOL;
   if (!EquipmentData.EquipmentSlot) {
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
      R_LOG_PRINTF ("Incompatable Equipment Slot and Item Slot Class: Slot [%s] Item [%s]",
         *EquipmentSlot->GetClass ()->GetName (),
         *EquipmentData.EquipmentSlot->GetName ()
         );
   }

   URStatusMgrComponent* StatusMgr = URUtil::GetComponent<URStatusMgrComponent> (GetOwner ());
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
      StatusMgr->SetPassiveEffects (EquipmentData.Name, EquipmentData.PassiveEffects);
      StatusMgr->AddResistance     (EquipmentData.Name, EquipmentData.Resistence);
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

   URStatusMgrComponent* StatusMgr = URUtil::GetComponent<URStatusMgrComponent> (GetOwner ());
   if (StatusMgr) {
      StatusMgr->RmPassiveEffects (EquipmentSlot->EquipmentData.Name);
      StatusMgr->RmResistance     (EquipmentSlot->EquipmentData.Name);
   }
   EquipmentSlot->Busy = false;
   EquipmentSlot->EquipmentData = FREquipmentData();
   EquipmentSlot->OnSlotUpdated.Broadcast ();
   OnEquipmentUpdated.Broadcast ();
   return true;
}

// ============================================================================
//                      Get Components
// ============================================================================

UREquipmentSlotComponent* UREquipmentMgrComponent::GetEquipmentSlot (const TSubclassOf<UREquipmentSlotComponent> SlotClass) const
{
   if (!SlotClass) return nullptr;
   TArray<UREquipmentSlotComponent*> CurrentEquipmentSlots;
   GetOwner ()->GetComponents (CurrentEquipmentSlots);

   UREquipmentSlotComponent *EquipmentSlot = nullptr;

   // --- Find equip slot
   for (UREquipmentSlotComponent* ItSlot : CurrentEquipmentSlots) {
      if (ItSlot->GetClass () == SlotClass) {

         // First slot available
         if (!EquipmentSlot) EquipmentSlot = ItSlot;

         // Empty Slot found
         if (!ItSlot->Busy) {
            EquipmentSlot = ItSlot;
            break;
         }
      }
   }
   return EquipmentSlot;
}

//=============================================================================
//                 Save / Load
//=============================================================================

void UREquipmentMgrComponent::OnSave (FBufferArchive &SaveData)
{
   Super::OnSave (SaveData);

   TArray<FString> EquipedItemsRaw;

   // --- Get equiped items
   TArray<UREquipmentSlotComponent*> CurrentEquipmentSlots;
   GetOwner ()->GetComponents (CurrentEquipmentSlots);
   for (UREquipmentSlotComponent* ItSlot : CurrentEquipmentSlots) {
      if (ItSlot->Busy) {
         FString RawData;
         if (RJSON::ToString (ItSlot->EquipmentData, RawData)) EquipedItemsRaw.Add (RawData);
      }
   }
   SaveData << EquipedItemsRaw;
}

void UREquipmentMgrComponent::OnLoad (FMemoryReader &LoadData)
{
   Super::OnLoad (LoadData);

   // --- Unequip current items
   TArray<UREquipmentSlotComponent*> CurrentEquipmentSlots;
   GetOwner ()->GetComponents (CurrentEquipmentSlots);
   for (UREquipmentSlotComponent* ItSlot : CurrentEquipmentSlots) {
      UnEquip (ItSlot);
   }

   TArray<FString> EquipedItemsRaw;
   LoadData << EquipedItemsRaw;

   // --- Equip Items
   for (const FString &ItRaw : EquipedItemsRaw) {
      FREquipmentData EquipData;
      if (RJSON::ToStruct (ItRaw, EquipData)) Equip (EquipData);
   }
}

