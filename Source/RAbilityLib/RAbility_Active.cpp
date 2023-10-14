// Copyright 2015-2023 Vagen Ayrapetyan

#include "RAbility_Active.h"
#include "RWorldAbilityMgr.h"

#include "RUtilLib/RUtil.h"
#include "RUtilLib/RLog.h"
#include "RUtilLib/RCheck.h"
#include "RUtilLib/RTimer.h"

//=============================================================================
//                 Active Ability
//=============================================================================

URAbility_Active::URAbility_Active ()
{
   PrimaryComponentTick.bCanEverTick = false;
   PrimaryComponentTick.bStartWithTickEnabled = false;
}

void URAbility_Active::BeginPlay ()
{
   Super::BeginPlay ();
   World = URUtil::GetWorld (this);
}

void URAbility_Active::AbilityInfoLoaded ()
{
   Super::AbilityInfoLoaded ();
}

void URAbility_Active::SetIsEnabled (bool Enabled)
{
   Super::SetIsEnabled (Enabled);

   if (!Enabled) CooldownReset ();
}

//=============================================================================
//                 Cooldown
//=============================================================================

double URAbility_Active::GetCooldownLeft () const
{
   // No need to check
   if (UseLastTime == 0) return 0;


   if (!World.IsValid () || World->bIsTearingDown) return 1;
   return FMath::Clamp (UseLastTime + Cooldown - World->GetTimeSeconds (), 0, Cooldown);
}

void URAbility_Active::CooldownReset ()
{
   UseLastTime = 0;
   RTIMER_STOP (CooldownResetHandle, this);
}

//=============================================================================
//                 Use
//=============================================================================

bool URAbility_Active::CanUse () const
{
   return GetIsEnabled () && GetCooldownLeft () == 0;
}

// Called by a person with keyboard
void URAbility_Active::Use ()
{
   if (!CanUse ()) return;

   if (R_IS_NET_ADMIN) Use_Global ();
   else                Use_Server ();
}

// Called on the authority
void URAbility_Active::Use_Server_Implementation ()
{
   if (!CanUse ()) return;

   Use_Global ();
}

// Called on all instances
void URAbility_Active::Use_Global_Implementation ()
{
   if (!World.IsValid () || World->bIsTearingDown) return;

   UseLastTime = World->GetTimeSeconds ();

   // Report can use
   RTIMER_START (CooldownResetHandle, this, &URAbility_Active::CooldownReset, Cooldown, false);

   // Report used
   if (OnAbilityUsed.IsBound ()) OnAbilityUsed.Broadcast ();
   if (WorldAbilityMgr.IsValid ()) WorldAbilityMgr->ReportUseAbility (this);
}

