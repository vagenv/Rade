// Copyright 2015-2023 Vagen Ayrapetyan

#include "RAbilityTypes.h"
#include "RUtilLib/RUtil.h"
#include "RUtilLib/RLog.h"
#include "RUtilLib/RCheck.h"

//=============================================================================
//                 Abilities
//=============================================================================

URAbility::URAbility ()
{
   PrimaryComponentTick.bCanEverTick = true;
   PrimaryComponentTick.bStartWithTickEnabled = true;
   PrimaryComponentTick.TickInterval = 0.2;
   SetIsReplicatedByDefault (true);
}

void URAbility::BeginPlay ()
{
   Super::BeginPlay ();
   const UWorld *world = GetWorld ();
   if (!ensure (world)) return;

   if (R_IS_NET_ADMIN) {

   }
}

void URAbility::EndPlay (const EEndPlayReason::Type EndPlayReason)
{
   Super::EndPlay (EndPlayReason);
}

void URAbility::TickComponent (float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction *ThisTickFunction)
{
   Super::TickComponent (DeltaTime, TickType, ThisTickFunction);

   // Decrease cooldown
   if (CooldownLeft > 0) {
      CooldownLeft = FMath::Clamp (CooldownLeft - DeltaTime, 0 , Cooldown);
   }

   // CanUse may be overloaded
   if (UseBlocked && CanUse ()) {
      UseBlocked = false;
      OnAbilityStatusUpdated.Broadcast ();
   }
}

void URAbility::Use ()
{
   UseBlocked = true;
   CooldownLeft = Cooldown;
   OnAbilityStatusUpdated.Broadcast ();
}

bool URAbility::CanUse () const
{
   return GetCooldownLeft () == 0;
}

double URAbility::GetCooldownLeft () const
{
   return CooldownLeft;
}

