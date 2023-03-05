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
}

void URAbility::Use ()
{
   UsedLast = FPlatformTime::Seconds ();
}

bool URAbility::CanUse () const
{
   return FPlatformTime::Seconds () > UsedLast + Cooldown;
}

double URAbility::GetCooldownLeft () const
{
   if (!Cooldown) return 0;
   if (!UsedLast) return 0;
   double dt = UsedLast + Cooldown - FPlatformTime::Seconds ();
   if (dt < 0) dt = 0;
   return dt;
}

