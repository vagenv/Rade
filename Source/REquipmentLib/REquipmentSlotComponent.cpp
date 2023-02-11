// Copyright 2015-2023 Vagen Ayrapetyan

#include "REquipmentSlotComponent.h"
#include "RUtilLib/RLog.h"
#include "Net/UnrealNetwork.h"

//=============================================================================
//                 Core
//=============================================================================

UREquipmentSlotComponent::UREquipmentSlotComponent ()
{
   SetIsReplicatedByDefault (true);
}

// Replication
void UREquipmentSlotComponent::GetLifetimeReplicatedProps (TArray<FLifetimeProperty> &OutLifetimeProps) const
{
   Super::GetLifetimeReplicatedProps (OutLifetimeProps);
   DOREPLIFETIME (UREquipmentSlotComponent, Busy);
   DOREPLIFETIME (UREquipmentSlotComponent, EquipmentData);
}

void UREquipmentSlotComponent::OnRep_Slot ()
{
   OnSlotUpdated.Broadcast ();
}

void UREquipmentSlotComponent::BeginPlay()
{
   Super::BeginPlay();
}

void UREquipmentSlotComponent::EndPlay (const EEndPlayReason::Type EndPlayReason)
{
   Super::EndPlay (EndPlayReason);
}

