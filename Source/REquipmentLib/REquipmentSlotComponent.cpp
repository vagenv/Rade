// Copyright 2015-2023 Vagen Ayrapetyan

#include "REquipmentSlotComponent.h"
#include "RUtilLib/RLog.h"
#include "RUtilLib/RCheck.h"
#include "Net/UnrealNetwork.h"

//=============================================================================
//                 Core
//=============================================================================

UREquipmentSlotComponent::UREquipmentSlotComponent ()
{
   SetIsReplicatedByDefault (true);
}

void UREquipmentSlotComponent::GetLifetimeReplicatedProps (TArray<FLifetimeProperty> &OutLifetimeProps) const
{
   Super::GetLifetimeReplicatedProps (OutLifetimeProps);
   DOREPLIFETIME (UREquipmentSlotComponent, Busy);
   DOREPLIFETIME (UREquipmentSlotComponent, EquipmentData);
}

void UREquipmentSlotComponent::OnRep_Slot ()
{
   ReportOnSlotUpdated ();
}

void UREquipmentSlotComponent::BeginPlay()
{
   Super::BeginPlay();
}

void UREquipmentSlotComponent::EndPlay (const EEndPlayReason::Type EndPlayReason)
{
   Super::EndPlay (EndPlayReason);
}

void UREquipmentSlotComponent::ReportOnSlotUpdated ()
{
   if (R_IS_VALID_WORLD && OnSlotUpdated.IsBound ()) OnSlotUpdated.Broadcast ();
}

