// Copyright 2015-2023 Vagen Ayrapetyan

#include "RAbility_Active.h"
#include "RWorldAbilityMgr.h"

#include "RUtilLib/RUtil.h"
#include "RUtilLib/RLog.h"
#include "RUtilLib/RCheck.h"

//=============================================================================
//                 Active Ability
//=============================================================================

URAbility_Active::URAbility_Active ()
{
   PrimaryComponentTick.bCanEverTick = true;
   PrimaryComponentTick.bStartWithTickEnabled = true;
   PrimaryComponentTick.TickInterval = 0.5f;
}

void URAbility_Active::BeginPlay ()
{
   Super::BeginPlay ();
   World = URUtil::GetWorld (this);
}

void URAbility_Active::TickComponent (float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction *ThisTickFunction)
{
   Super::TickComponent (DeltaTime, TickType, ThisTickFunction);

   if (!IsUseable) {

      if (World.IsValid () && !World->bIsTearingDown) {
         // Update cooldown left
         UseCooldownLeft = FMath::Clamp (UseLastTime + Cooldown - World->GetTimeSeconds (), 0, Cooldown);

         // Can be used
         if (UseCooldownLeft == 0 && GetIsEnabled ()) {
            IsUseable = true;
            if (R_IS_VALID_WORLD && OnAbilityStatusUpdated.IsBound ()) OnAbilityStatusUpdated.Broadcast ();
         }
      }
   }
}

bool URAbility_Active::CanUse () const
{
   return GetIsEnabled () && IsUseable;
}

double URAbility_Active::GetCooldownLeft () const
{
   return UseCooldownLeft;
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
   if (!World.IsValid ()) return;

   // Update local state
   UseLastTime     = World->GetTimeSeconds ();
   UseCooldownLeft = Cooldown;
   IsUseable       = false;

   if (WorldAbilityMgr.IsValid ()) WorldAbilityMgr->ReportUseAbility (this);

   // Broadcast event
   if (R_IS_VALID_WORLD) {
      if (OnAbilityUsed.IsBound ())          OnAbilityUsed.Broadcast ();
      if (OnAbilityStatusUpdated.IsBound ()) OnAbilityStatusUpdated.Broadcast ();
   }
}

