// Copyright 2015-2023 Vagen Ayrapetyan

#include "RActiveStatusEffect.h"
#include "RStatusMgrComponent.h"
#include "RWorldStatusMgr.h"

#include "RUtilLib/RUtil.h"
#include "RUtilLib/RLog.h"
#include "RUtilLib/RCheck.h"

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

   // Find instance data on balancer
   if (URWorldStatusMgr* WorldMgr = URWorldStatusMgr::GetInstance (this)) {
      EffectInfo = WorldMgr->GetEffectInfo (this);
   }

   if (!EffectInfo.IsValid ()) {
      R_LOG_PRINTF ("Error. [%s] Effect info is invalid.", *GetPathName ());
   }

   Started ();
}

void URActiveStatusEffect::EndPlay (const EEndPlayReason::Type EndPlayReason)
{
   if (GetOwner ()->GetInstanceComponents ().Contains (this))
      GetOwner ()->RemoveInstanceComponent (this);

   Ended ();
   Super::EndPlay (EndPlayReason);
}

//=============================================================================
//                 Start/Stop/Refresh/End
//=============================================================================

void URActiveStatusEffect::Started ()
{
   // --- Check Values
   if (!ensure (GetWorld ())) return;
   URStatusMgrComponent* StatusMgr_ = URUtil::GetComponent<URStatusMgrComponent> (GetOwner ());
   if (!ensure (StatusMgr_)) return;
   StatusMgr = StatusMgr_;

   IsRunning = true;

   Apply ();

   // --- Report

   if (URWorldStatusMgr* WorldMgr = URWorldStatusMgr::GetInstance (this)) {
      WorldMgr->ReportStatusEffectStart (this);
   }

   StatusMgr->ReportActiveEffectsUpdated ();
   if (R_IS_VALID_WORLD && OnStart.IsBound ()) OnStart.Broadcast ();
}

void URActiveStatusEffect::Stop ()
{
   UWorld* World = GetWorld ();
   if (!ensure (World)) return;
   if (TimerToEnd.IsValid ()) World->GetTimerManager ().ClearTimer (TimerToEnd);
   if (R_IS_VALID_WORLD && OnCancel.IsBound ()) OnCancel.Broadcast ();
   DestroyComponent ();
}

void URActiveStatusEffect::Refresh_Implementation ()
{
   UWorld* World = GetWorld ();
   if (!ensure (World)) return;
   if (TimerToEnd.IsValid ()) World->GetTimerManager ().ClearTimer (TimerToEnd);

   int StackMax = GetStackMax ();
   if (StackMax > 1 && StackCurrent < StackMax) {
      // int StackLast = StackCurrent;
      StackCurrent = FMath::Clamp (StackCurrent + 1, 1, StackMax);

      // // --- For debug
      // float StackLastScale    = URUtilLibrary::GetRuntimeFloatCurveValue (StackToScale, StackLast);
      // float StackCurrentScale = URUtilLibrary::GetRuntimeFloatCurveValue (StackToScale, StackCurrent);

      // R_LOG_PRINTF ("[%s] Effect stack increased [%d %.1f%] => [%d %.1f]",
      //    *GetName (),
      //    StackLast, StackLastScale * 100,
      //    StackCurrent, StackCurrentScale * 100);
   } else {
      // R_LOG_PRINTF ("[%s] Effect refreshed", *GetName ());
   }

   Apply ();

   // --- Report
   if (URWorldStatusMgr* WorldMgr = URWorldStatusMgr::GetInstance (this)) {
      WorldMgr->ReportStatusEffectRefresh (this);
   }
   if (R_IS_VALID_WORLD && OnRefresh.IsBound ()) OnRefresh.Broadcast ();
}

void URActiveStatusEffect::Ended ()
{
   IsRunning = false;

   // --- Report
   if (URWorldStatusMgr* WorldMgr = URWorldStatusMgr::GetInstance (this)) {
      WorldMgr->ReportStatusEffectEnd (this);
   }
   if (IsValid (StatusMgr)) {
      if (R_IS_NET_ADMIN) StatusMgr->RmPassiveEffects (EffectInfo.Description.Label);
      StatusMgr->ReportActiveEffectsUpdated ();
   }

   if (R_IS_VALID_WORLD && OnEnd.IsBound ()) OnEnd.Broadcast ();
}

void URActiveStatusEffect::Apply ()
{
   UWorld* World = GetWorld ();
   if (!ensure (World)) return;
   StartTime = World->GetTimeSeconds ();

   if (R_IS_NET_ADMIN) {
      if (IsValid (StatusMgr)) {
         float StackScale = GetStackScale ();
         TArray<FRPassiveStatusEffect> CopyPassiveEffects = EffectInfo.PassiveEffects;
         for (FRPassiveStatusEffect &ItPassiveEffect : CopyPassiveEffects) {
            ItPassiveEffect.Value = ItPassiveEffect.Value * StackScale;
         }
         StatusMgr->SetPassiveEffects (EffectInfo.Description.Label, CopyPassiveEffects);
      }
   }

   if (R_IS_NET_ADMIN && EffectInfo.Duration > 0) {
      World->GetTimerManager ().SetTimer (TimerToEnd, this, &URActiveStatusEffect::Timeout, EffectInfo.Duration, false);
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
   UWorld* World = GetWorld ();
   if (!ensure (World)) return 0;
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

FRUIDescription URActiveStatusEffect::GetDescription () const
{
   return EffectInfo.Description;
}

//=============================================================================
//                 Status Effect Library
//=============================================================================

bool URStatusEffectUtilLibrary::SetStatusEffect_Passive (
   AActor *Target,
   const FString &Tag,
   const TArray<FRPassiveStatusEffect> &Effects)
{
   // --- Check Values
   if (!ensure (Target))                  return false;
   if (!ensure (Target->HasAuthority ())) return false;
   if (!ensure (!Tag.IsEmpty ()))         return false;
   URStatusMgrComponent* StatusMgr = URUtil::GetComponent<URStatusMgrComponent> (Target);
   if (!ensure (StatusMgr))               return false;

   // --- Action
   return StatusMgr->SetPassiveEffects (Tag, Effects);
}

bool URStatusEffectUtilLibrary::RmStatusEffect_Passive (
   AActor *Target,
   const FString &Tag)
{
   // --- Check Values
   if (!ensure (Target))                  return false;
   if (!ensure (Target->HasAuthority ())) return false;
   if (!ensure (!Tag.IsEmpty ()))         return false;
   URStatusMgrComponent* StatusMgr = URUtil::GetComponent<URStatusMgrComponent> (Target);
   if (!ensure (StatusMgr))               return false;

   // --- Action
   return StatusMgr->RmPassiveEffects (Tag);
}

bool URStatusEffectUtilLibrary::ApplyStatusEffect_Active (
   AActor* Causer,
   AActor *Target,
   const TSubclassOf<URActiveStatusEffect> Effect_)
{
   // --- Check Values
   if (!ensure (Causer))                  return false;
   if (!ensure (Target))                  return false;
   if (!ensure (Effect_))                 return false;

   UWorld* World = Target->GetWorld ();
   if (!World)                            return false;
   URStatusMgrComponent* StatusMgr = URUtil::GetComponent<URStatusMgrComponent> (Target);
   if (!ensure (StatusMgr))               return false;

   return StatusMgr->AddActiveStatusEffect (Causer, Effect_);
}

