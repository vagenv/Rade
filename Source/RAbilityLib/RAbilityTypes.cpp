// Copyright 2015-2023 Vagen Ayrapetyan

#include "RAbilityTypes.h"
#include "RUtilLib/RUtil.h"
#include "RUtilLib/RLog.h"
#include "RUtilLib/RCheck.h"


//=============================================================================
//                 Passsive Ability
//=============================================================================

URAbility_Passive::URAbility_Passive ()
{
   PrimaryComponentTick.bCanEverTick = false;
   PrimaryComponentTick.bStartWithTickEnabled = false;
}

//=============================================================================
//                 Active Ability
//=============================================================================

URAbility_Active::URAbility_Active ()
{
   PrimaryComponentTick.bCanEverTick = true;
   PrimaryComponentTick.bStartWithTickEnabled = true;
   PrimaryComponentTick.TickInterval = 0.2;
}

void URAbility_Active::TickComponent (float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction *ThisTickFunction)
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

void URAbility_Active::Use ()
{
   if (!CanUse ()) return;
   UseBlocked = true;
   CooldownLeft = Cooldown;
   OnAbilityStatusUpdated.Broadcast ();
   OnAbilityUsed.Broadcast ();
}

bool URAbility_Active::CanUse () const
{
   return CooldownLeft == 0;
}

