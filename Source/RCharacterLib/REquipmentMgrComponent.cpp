// Copyright 2015-2023 Vagen Ayrapetyan

#include "REquipmentMgrComponent.h"
#include "RUtilLib/Rlog.h"
#include "REquipmentTypes.h"

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

   FREquipmentData ItemData;
   if (!FREquipmentData::Cast (Items[ItemIdx], ItemData)) {
      return Super::UseItem (ItemIdx);
   }

   R_LOG_PRINTF ("Equipment item used. Item: %s Socket : [%s]", *ItemData.Name, *ItemData.AttachSocket);

   // ---
   // Try equip item
   // Check required stats
   // Add addition stats and effects
   // ---


   // --- Item Action
   // URItemAction *ItemBP = ItemData.Action->GetDefaultObject<URItemAction>();
   // if (!ensure (ItemBP)) return false;
   // ItemBP->Used (this, ItemData, ItemIdx);
   // BP_Used (ItemIdx);



   return false;
}

