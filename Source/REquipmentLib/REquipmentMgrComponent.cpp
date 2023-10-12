// Copyright 2015-2023 Vagen Ayrapetyan

#include "REquipmentMgrComponent.h"
#include "REquipmentTypes.h"
#include "REquipmentSlotComponent.h"

#include "RUtilLib/RUtil.h"
#include "RUtilLib/RLog.h"
#include "RUtilLib/RCheck.h"
#include "RUtilLib/RJson.h"

#include "RStatusLib/RPlayerStatusMgrComponent.h"
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
      if (URPlayerStatusMgrComponent *StatusMgr = URUtil::GetComponent<URPlayerStatusMgrComponent> (GetOwner ())) {
         StatusMgr->OnStatsUpdated.AddDynamic (this, &UREquipmentMgrComponent::OnStatsUpdated);
      }

      FTimerHandle TempHandle;
      GetOwner ()->GetWorldTimerManager ().SetTimer (TempHandle,
                                                     this, &UREquipmentMgrComponent::OnStatsUpdated,
                                                     1);
   }
}

void UREquipmentMgrComponent::EndPlay (const EEndPlayReason::Type EndPlayReason)
{
   Super::EndPlay (EndPlayReason);
}

UREquipmentSlotComponent* UREquipmentMgrComponent::GetEquipmentSlot (const TSoftClassPtr<UREquipmentSlotComponent> SlotClass) const
{
   if (!ensure (!SlotClass.IsNull ())) return nullptr;
   TArray<UREquipmentSlotComponent*> CurrentEquipmentSlots;
   GetOwner ()->GetComponents (CurrentEquipmentSlots);

   UREquipmentSlotComponent *EquipmentSlot = nullptr;

   // --- Find equip slot
   for (UREquipmentSlotComponent* ItSlot : CurrentEquipmentSlots) {
      if (!IsValid (ItSlot)) continue;
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

void UREquipmentMgrComponent::ReportEquipmentUpdated ()
{
   if (R_IS_VALID_WORLD && OnEquipmentUpdated.IsBound ()) OnEquipmentUpdated.Broadcast ();
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

   float EquipLoad = WeightCurrent * 100. / WeightMax;
   FRPassiveStatusEffect EvasionEffect;
   EvasionEffect.EffectTarget = ERStatusEffectTarget::Evasion;
   EvasionEffect.Percent      = URUtilLibrary::GetRuntimeFloatCurveValue (WeightToEvasion, EquipLoad);

   FRPassiveStatusEffect MoveSpeedEffect;
   MoveSpeedEffect.EffectTarget = ERStatusEffectTarget::MoveSpeed;
   MoveSpeedEffect.Percent      = URUtilLibrary::GetRuntimeFloatCurveValue (WeightToMoveSpeed, EquipLoad);

   TArray<FRPassiveStatusEffect> Effects;
   Effects.Add (EvasionEffect);
   Effects.Add (MoveSpeedEffect);

   StatusMgr->SetPassiveEffects ("Weight Penalty", Effects);
}

void UREquipmentMgrComponent::OnStatsUpdated ()
{
   URPlayerStatusMgrComponent *StatusMgr = URUtil::GetComponent<URPlayerStatusMgrComponent> (GetOwner ());
   if (StatusMgr) {
      FRCoreStats StatsTotal = StatusMgr->GetCoreStats_Total ();
      WeightMax = URUtilLibrary::GetRuntimeFloatCurveValue (StrToWeightMax, StatsTotal.STR);
      if (LastWeightMax != WeightMax) {
         LastWeightMax = WeightMax;
         CalcWeight ();
      }
   }
}

//=============================================================================
//                 Use / Drop override
//=============================================================================

bool UREquipmentMgrComponent::UseItem_Index (int32 ItemIdx)
{
   R_RETURN_IF_NOT_ADMIN_BOOL;

   // Valid index
   if (!Items.IsValidIndex (ItemIdx)) {
      R_LOG_PRINTF ("Invalid Item Index [%d]. Must be [0-%d]",
         ItemIdx, Items.Num ());
      return false;
   }

   FRItemData ItemData = Items[ItemIdx];

   if (FRConsumableItemData::CanCast (ItemData))  {
      FRConsumableItemData ConsumeData;
      if (FRConsumableItemData::Cast (ItemData, ConsumeData)) {
         if (!ConsumeData.Used (GetOwner (), this)) return false;
         if (ConsumeData.DestroyOnAction) return RemoveItem_Index (ItemIdx, 1);
         return true;
      }
   }

   // --- Not an equipment item. Pass management to Inventory.
   if (!FREquipmentData::CanCast (ItemData)) {
      return Super::UseItem_Index (ItemIdx);
   }

   FREquipmentData EquipmentData;
   if (!FREquipmentData::Cast (ItemData, EquipmentData)) {
      return Super::UseItem_Index (ItemIdx);
   }

   bool success = Equip_Equipment (EquipmentData);

   // --- If Custom Action is defined
   if (success && !EquipmentData.Action.IsNull ()) {
      if (!EquipmentData.Used (GetOwner (), this)) return false;
   }
   return success;
}

bool UREquipmentMgrComponent::DropItem_Index (int32 ItemIdx, int32 Count)
{
   R_RETURN_IF_NOT_ADMIN_BOOL;

   // --- Check if Item should be unequiped
   FREquipmentData EquipmentData;
   // Is Equipment item
   if (FREquipmentData::Cast (Items[ItemIdx], EquipmentData)) {

      // Get target slot type
      UREquipmentSlotComponent *EquipmentSlot = GetEquipmentSlot (EquipmentData.EquipmentSlot);

      if (EquipmentSlot) {

         // Item equiped
         if (EquipmentSlot->EquipmentData.ID == EquipmentData.ID) {
            UnEquip_Slot (EquipmentSlot);
         }
      }
   }

   return Super::DropItem_Index (ItemIdx, Count);
}

bool UREquipmentMgrComponent::BreakItem_Index (int32 ItemIdx, const UDataTable* BreakItemTable)
{
   R_RETURN_IF_NOT_ADMIN_BOOL;

   // Valid index
   if (!Items.IsValidIndex (ItemIdx)) {
      R_LOG_PRINTF ("Invalid Item Index [%d]. Must be [0-%d]",
         ItemIdx, Items.Num ());
      return false;
   }

   // Unequip, if equiped.
   UnEquip_Index (ItemIdx);

   return Super::BreakItem_Index (ItemIdx, BreakItemTable);
}

//=============================================================================
//                 Is Equiped
//=============================================================================

bool UREquipmentMgrComponent::IsEquiped_Index (int32 ItemIdx)
{
   R_RETURN_IF_NOT_ADMIN_BOOL;

   // Valid index
   if (!Items.IsValidIndex (ItemIdx)) {
      R_LOG_PRINTF ("Invalid Item Index [%d]. Must be [0-%d]",
         ItemIdx, Items.Num ());
      return false;
   }

   return IsEquiped_Item (Items[ItemIdx]);
}

bool UREquipmentMgrComponent::IsEquiped_Item (const FRItemData &ItemData)
{
   if (!ItemData.IsValid ()) return false;
   if (!FREquipmentData::CanCast (ItemData)) return false;

   FREquipmentData EquipmentData;
   if (FREquipmentData::Cast (ItemData, EquipmentData)) {
      return IsEquiped_Equipment (EquipmentData);
   }
   return false;
}

bool UREquipmentMgrComponent::IsEquiped_Equipment (const FREquipmentData &EquipmentData)
{
   if (!EquipmentData.IsValid ()) return false;
   // Get target slot type
   UREquipmentSlotComponent *EquipmentSlot = GetEquipmentSlot (EquipmentData.EquipmentSlot);
   if (!EquipmentSlot) return false;
   return EquipmentSlot->EquipmentData.ID == EquipmentData.ID;
}

//=============================================================================
//                 Equip
//=============================================================================

void UREquipmentMgrComponent::Equip_Index_Server_Implementation (
   UREquipmentMgrComponent *DstEquipment, int32 ItemIdx)
{
   if (!ensure (DstEquipment)) return;
   DstEquipment->Equip_Index (ItemIdx);
}

bool UREquipmentMgrComponent::Equip_Index (int32 ItemIdx)
{
   R_RETURN_IF_NOT_ADMIN_BOOL;

   // Valid index
   if (!Items.IsValidIndex (ItemIdx)) {
      R_LOG_PRINTF ("Invalid Item Index [%d]. Must be [0-%d]",
         ItemIdx, Items.Num ());
      return false;
   }

   return Equip_Item (Items[ItemIdx]);
}

void UREquipmentMgrComponent::Equip_Item_Server_Implementation (
   UREquipmentMgrComponent *DstEquipment, const FRItemData &ItemData)
{
   if (!ensure (DstEquipment)) return;
   DstEquipment->Equip_Item (ItemData);
}

bool UREquipmentMgrComponent::Equip_Item (const FRItemData &ItemData)
{
   if (!ItemData.IsValid ()) return false;
   if (!FREquipmentData::CanCast (ItemData)) return false;

   FREquipmentData EquipmentData;
   if (FREquipmentData::Cast (ItemData, EquipmentData)) {
      return Equip_Equipment (EquipmentData);
   }
   return false;
}

void UREquipmentMgrComponent::Equip_Equipment_Server_Implementation (
   UREquipmentMgrComponent *DstEquipment, const FREquipmentData &EquipmentData)
{
   if (!ensure (DstEquipment)) return;
   DstEquipment->Equip_Equipment (EquipmentData);
}

bool UREquipmentMgrComponent::Equip_Equipment (const FREquipmentData &EquipmentData)
{
   R_RETURN_IF_NOT_ADMIN_BOOL;
   if (!EquipmentData.IsValid ()) return false;

   if (!EquipmentData.EquipmentSlot) {
      R_LOG_PRINTF ("Equipment item [%s] doesn't have a valid equip slot set.", *EquipmentData.ID);
      return false;
   }

   UREquipmentSlotComponent *EquipmentSlot = GetEquipmentSlot (EquipmentData.EquipmentSlot);
   if (!EquipmentSlot) {
      R_LOG_PRINTF ("Equipment item [%s] required slot [%s] not found on character",
         *EquipmentData.ID, *EquipmentData.EquipmentSlot->GetName ());
      return false;
   }
   return Equip_Slot (EquipmentSlot, EquipmentData);
}

void UREquipmentMgrComponent::Equip_Slot_Server_Implementation (
   UREquipmentMgrComponent  *DstEquipment,
   UREquipmentSlotComponent *EquipmentSlot,
   const FREquipmentData    &EquipmentData)
{
   if (!ensure (DstEquipment)) return;
   DstEquipment->Equip_Slot (EquipmentSlot, EquipmentData);
}

bool UREquipmentMgrComponent::Equip_Slot (UREquipmentSlotComponent *EquipmentSlot, const FREquipmentData &EquipmentData)
{
   R_RETURN_IF_NOT_ADMIN_BOOL;
   if (!ensure (EquipmentSlot))  return false;
   if (!EquipmentData.IsValid ()) return false;
   if (!ensure (!EquipmentData.EquipmentSlot.IsNull ())) {
      R_LOG_PRINTF ("Equipment item [%s] doesn't have a valid equip slot set.", *EquipmentData.ID);
      return false;
   }
   if (!ensure (EquipmentSlot->GetClass () == EquipmentData.EquipmentSlot)) {
      R_LOG_PRINTF ("Incompatable Equipment Slot and Item Slot Class: Slot [%s] Item [%s]",
         *EquipmentSlot->GetClass ()->GetName (),
         *EquipmentData.EquipmentSlot->GetName ()
         );
      return false;
   }

   URPlayerStatusMgrComponent* StatusMgr = URUtil::GetComponent<URPlayerStatusMgrComponent> (GetOwner ());
   if (!EquipmentData.RequiredStats.IsEmpty ()) {
      if (!StatusMgr) {
         R_LOG_PRINTF ("Equipment item [%s] failed. URStatusMgrComponent not found", *EquipmentData.ID);
         return false;
      }
      if (!StatusMgr->HasStats (EquipmentData.RequiredStats)) return false;
   }

   // --- Unequip if busy
   if (EquipmentSlot->Busy) {

      // If equip has been called on same item -> Only unequip.
      if (EquipmentSlot->EquipmentData.ID == EquipmentData.ID) {
         return UnEquip_Slot (EquipmentSlot);
      }

      if (!UnEquip_Slot (EquipmentSlot)) return false;
   }

   // --- Add Stats and Effects
   if (StatusMgr) {
      StatusMgr->SetPassiveEffects (EquipmentSlot->Description.Label, EquipmentData.PassiveEffects);
      StatusMgr->AddResistance     (EquipmentSlot->Description.Label, EquipmentData.Resistence);
   }

   // --- Update slot data
   EquipmentSlot->EquipmentData = EquipmentData;
   EquipmentSlot->Busy = true;
   EquipmentSlot->ReportOnSlotUpdated ();
   ReportEquipmentUpdated ();

   return true;
}

//=============================================================================
//                 Unequip
//=============================================================================

void UREquipmentMgrComponent::UnEquip_Index_Server_Implementation (
   UREquipmentMgrComponent *DstEquipment, int32 ItemIdx)
{
   if (!ensure (DstEquipment)) return;
   DstEquipment->UnEquip_Index (ItemIdx);
}

bool UREquipmentMgrComponent::UnEquip_Index (int32 ItemIdx)
{
   R_RETURN_IF_NOT_ADMIN_BOOL;

   // Valid index
   if (!Items.IsValidIndex (ItemIdx)) {
      R_LOG_PRINTF ("Invalid Item Index [%d]. Must be [0-%d]",
         ItemIdx, Items.Num ());
      return false;
   }

   return UnEquip_Item (Items[ItemIdx]);
}

void UREquipmentMgrComponent::UnEquip_Item_Server_Implementation (
   UREquipmentMgrComponent *DstEquipment, const FRItemData &ItemData)
{
   if (!ensure (DstEquipment)) return;
   DstEquipment->UnEquip_Item (ItemData);
}

bool UREquipmentMgrComponent::UnEquip_Item (const FRItemData &ItemData)
{
   R_RETURN_IF_NOT_ADMIN_BOOL;
   if (!ItemData.IsValid ()) return false;
   if (!FREquipmentData::CanCast (ItemData)) return false;

   FREquipmentData EquipmentData;
   if (FREquipmentData::Cast (ItemData, EquipmentData)) {
      return UnEquip_Equipment (EquipmentData);
   }
   return false;
}



void UREquipmentMgrComponent::UnEquip_Equipment_Server_Implementation (
   UREquipmentMgrComponent *DstEquipment, const FREquipmentData &EquipmentData)
{
   if (!ensure (DstEquipment)) return;
   DstEquipment->UnEquip_Equipment (EquipmentData);
}

bool UREquipmentMgrComponent::UnEquip_Equipment (const FREquipmentData &EquipmentData)
{
   R_RETURN_IF_NOT_ADMIN_BOOL;

   if (!EquipmentData.IsValid ()) return false;

   // Get target slot type
   UREquipmentSlotComponent *EquipmentSlot = GetEquipmentSlot (EquipmentData.EquipmentSlot);
   if (EquipmentSlot) {
      if (EquipmentSlot->EquipmentData.ID == EquipmentData.ID) {
         return UnEquip_Slot (EquipmentSlot);
      }
   }
   return false;
}

void UREquipmentMgrComponent::UnEquip_Slot_Server_Implementation (
   UREquipmentMgrComponent  *DstEquipment,
   UREquipmentSlotComponent *EquipmentSlot)
{
   if (!ensure (DstEquipment)) return;
   DstEquipment->UnEquip_Slot (EquipmentSlot);
}

bool UREquipmentMgrComponent::UnEquip_Slot (UREquipmentSlotComponent *EquipmentSlot)
{
   R_RETURN_IF_NOT_ADMIN_BOOL;
   if (!ensure (EquipmentSlot)) return false;
   if (!EquipmentSlot->Busy)    return false;

   URStatusMgrComponent* StatusMgr = URUtil::GetComponent<URStatusMgrComponent> (GetOwner ());
   if (StatusMgr) {
      StatusMgr->RmPassiveEffects (EquipmentSlot->Description.Label);
      StatusMgr->RmResistance     (EquipmentSlot->Description.Label);
   }
   EquipmentSlot->Busy = false;
   EquipmentSlot->EquipmentData = FREquipmentData();
   EquipmentSlot->ReportOnSlotUpdated ();
   ReportEquipmentUpdated ();

   return true;
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
   for (const UREquipmentSlotComponent* ItSlot : CurrentEquipmentSlots) {
      if (IsValid (ItSlot) && ItSlot->Busy) {
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
      UnEquip_Slot (ItSlot);
   }

   TArray<FString> EquipedItemsRaw;
   LoadData << EquipedItemsRaw;

   // --- Equip Items
   for (const FString &ItRaw : EquipedItemsRaw) {
      FREquipmentData EquipData;
      if (RJSON::ToStruct (ItRaw, EquipData)) Equip_Equipment (EquipData);
   }
}

