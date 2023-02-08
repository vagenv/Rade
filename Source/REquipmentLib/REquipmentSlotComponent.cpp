// Copyright 2015-2023 Vagen Ayrapetyan

#include "REquipmentSlotComponent.h"
#include "RUtilLib/RLog.h"

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
}

void UREquipmentSlotComponent::BeginPlay()
{
   Super::BeginPlay();
}

void UREquipmentSlotComponent::EndPlay (const EEndPlayReason::Type EndPlayReason)
{
   Super::EndPlay (EndPlayReason);
}

