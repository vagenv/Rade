// Copyright 2015-2023 Vagen Ayrapetyan

#include "RActiveStatusEffect.h"
#include "RStatusMgrComponent.h"
#include "RWorldStatusMgr.h"

#include "RUtilLib/RUtil.h"
#include "RUtilLib/RLog.h"
#include "RUtilLib/RCheck.h"
#include "RUtilLib/RTimer.h"

#include "Net/UnrealNetwork.h"

//=============================================================================
//                 Active Effect
//=============================================================================

URActiveStatusEffect::URActiveStatusEffect ()
{
   SetIsReplicatedByDefault (true);
   PrimaryComponentTick.bCanEverTick = false;
   PrimaryComponentTick.bStartWithTickEnabled = false;
   bAutoRegister = true;
}

void URActiveStatusEffect::GetLifetimeReplicatedProps (TArray<FLifetimeProperty> &OutLifetimeProps) const
{
   Super::GetLifetimeReplicatedProps (OutLifetimeProps);
   DOREPLIFETIME (URActiveStatusEffect, Causer);
}

//=============================================================================
//                 System hooks
//=============================================================================

void URActiveStatusEffect::BeginPlay ()
{
   Super::BeginPlay ();

   if (!GetOwner ()->GetInstanceComponents ().Contains (this))
      GetOwner ()->AddInstanceComponent (this);

   OwnerStatusMgr = URUtil::GetComponent<URStatusMgrComponent> (GetOwner ());

   FindWorldStatusMgr ();
}

void URActiveStatusEffect::EndPlay (const EEndPlayReason::Type EndPlayReason)
{
   if (GetOwner ()->GetInstanceComponents ().Contains (this))
      GetOwner ()->RemoveInstanceComponent (this);

   Ended ();
   Super::EndPlay (EndPlayReason);
}

void URActiveStatusEffect::FindWorldStatusMgr ()
{
   WorldStatusMgr = URWorldStatusMgr::GetInstance (this);
   if (!WorldStatusMgr.IsValid ()) {
      FTimerHandle RetryHandle;
      RTIMER_START (RetryHandle,
                    this, &URActiveStatusEffect::FindWorldStatusMgr,
                    1, false);
      return;
   }

   // Get latest balance from table
   EffectInfo = WorldStatusMgr->GetEffectInfo (this);

   Started ();
}

//=============================================================================
//                 Start / Stop / Refresh / End
//=============================================================================

void URActiveStatusEffect::Started ()
{
   IsRunning = true;

   // --- Apply effect
   Apply ();

   // --- Report
   if (R_IS_VALID_WORLD) {
      if (WorldStatusMgr.IsValid ()) WorldStatusMgr->ReportStatusEffectStart (this);
      if (OwnerStatusMgr.IsValid ()) OwnerStatusMgr->ReportActiveEffectsUpdated ();
      if (OnStart.IsBound ()) OnStart.Broadcast ();
   }
}

void URActiveStatusEffect::Stop ()
{
   if (R_IS_VALID_WORLD) {
      if (WorldStatusMgr.IsValid ()) WorldStatusMgr->ReportStatusEffectStop (this);
      if (OnStop.IsBound ()) OnStop.Broadcast ();
   }

   DestroyComponent ();
}

void URActiveStatusEffect::Refresh_Implementation ()
{
   RTIMER_STOP (TimeoutHandle, this);

   int StackMax = GetStackMax ();
   if (StackMax > 1 && StackCurrent < StackMax) {
      // int StackLast = StackCurrent;
      StackCurrent = FMath::Clamp (StackCurrent + 1, 1, StackMax);

      // // --- For debug
      // float StackLastScale    = URUtil::GetRuntimeFloatCurveValue (StackToScale, StackLast);
      // float StackCurrentScale = URUtil::GetRuntimeFloatCurveValue (StackToScale, StackCurrent);

      // R_LOG_PRINTF ("[%s] Effect stack increased [%d %.1f%] => [%d %.1f]",
      //    *GetName (),
      //    StackLast, StackLastScale * 100,
      //    StackCurrent, StackCurrentScale * 100);
   } else {
      // R_LOG_PRINTF ("[%s] Effect refreshed", *GetName ());
   }

   // --- Apply effect
   Apply ();

   // --- Report
   if (R_IS_VALID_WORLD) {
      if (WorldStatusMgr.IsValid ()) WorldStatusMgr->ReportStatusEffectRefresh (this);
      if (OnRefresh.IsBound ()) OnRefresh.Broadcast ();
   }
}

void URActiveStatusEffect::Ended ()
{
   IsRunning = false;

   // --- Report
   if (R_IS_VALID_WORLD) {
      if (WorldStatusMgr.IsValid ()) WorldStatusMgr->ReportStatusEffectEnd (this);

      if (OwnerStatusMgr.IsValid ()) {
         if (R_IS_NET_ADMIN) OwnerStatusMgr->RmPassiveEffects (EffectInfo.Description.Label);
         OwnerStatusMgr->ReportActiveEffectsUpdated ();
      }

      if (OnEnd.IsBound ()) OnEnd.Broadcast ();
   }

   RTIMER_STOP (TimeoutHandle, this);
}

void URActiveStatusEffect::Apply ()
{
   UWorld* World = URUtil::GetWorld (this);
   if (!World) return;
   StartTime = World->GetTimeSeconds ();

   if (R_IS_NET_ADMIN) {
      if (OwnerStatusMgr.IsValid ()) {
         float StackScale = GetStackScale ();
         TArray<FRPassiveStatusEffect> CopyPassiveEffects = EffectInfo.PassiveEffects;
         for (FRPassiveStatusEffect &ItPassiveEffect : CopyPassiveEffects) {
            ItPassiveEffect.Flat    = ItPassiveEffect.Flat    * StackScale;
            ItPassiveEffect.Percent = ItPassiveEffect.Percent * StackScale;
         }
         OwnerStatusMgr->SetPassiveEffects (EffectInfo.Description.Label, CopyPassiveEffects);
      }
   }

   if (R_IS_NET_ADMIN && EffectInfo.Duration > 0) {
      RTIMER_START (TimeoutHandle,
                    this, &URActiveStatusEffect::Timeout,
                    EffectInfo.Duration, false);
   }
}

void URActiveStatusEffect::Timeout ()
{
   DestroyComponent ();
}

//=============================================================================
//                 Status Functions
//=============================================================================

double URActiveStatusEffect::GetDurationLeft () const
{
   UWorld* World = URUtil::GetWorld (this);
   if (!World) return 0;
   return FMath::Clamp (StartTime + EffectInfo.Duration - World->GetTimeSeconds (), 0, EffectInfo.Duration);
}

bool URActiveStatusEffect::GetIsRunning () const
{
   return IsRunning;
}

int URActiveStatusEffect::GetStackCurrent () const
{
   return StackCurrent;
}

int URActiveStatusEffect::GetStackMax () const
{
   int Result = 1;
   if (EffectInfo.StackScaling.Num () > 1) Result = EffectInfo.StackScaling.Num ();
   return Result;
}

float URActiveStatusEffect::GetStackScale (int Stack) const
{
   float Result = 1.;
   if (Stack < 0) Stack = GetStackCurrent ();
   Stack--;

   if (EffectInfo.StackScaling.IsValidIndex (Stack))
      Result = EffectInfo.StackScaling[Stack];
   return Result;
}

FRActiveStatusEffectInfo URActiveStatusEffect::GetEffectInfo () const
{
   return EffectInfo;
}

