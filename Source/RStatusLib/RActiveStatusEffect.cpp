// Copyright 2015-2023 Vagen Ayrapetyan

#include "RActiveStatusEffect.h"
#include "RStatusMgrComponent.h"
#include "RDamageMgr.h"

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

   FRichCurve* StackToScaleData = StackToScale.GetRichCurve ();
   StackToScaleData->AddKey (  1,   1); // Minimum
   StackToScaleData->AddKey ( 10,   5);
   StackToScaleData->AddKey ( 50,  15);
   StackToScaleData->AddKey (100,  20); // Maximum
}

void URActiveStatusEffect::GetLifetimeReplicatedProps (TArray<FLifetimeProperty> &OutLifetimeProps) const
{
   Super::GetLifetimeReplicatedProps (OutLifetimeProps);
   DOREPLIFETIME (URActiveStatusEffect, Causer);
}

void URActiveStatusEffect::OnComponentCreated ()
{
   Super::OnComponentCreated ();

   if (!GetOwner ()->GetInstanceComponents ().Contains (this))
      GetOwner ()->AddInstanceComponent (this);
}
void URActiveStatusEffect::OnComponentDestroyed (bool bDestroyingHierarchy)
{
   if (GetOwner ()->GetInstanceComponents ().Contains (this))
      GetOwner ()->RemoveInstanceComponent (this);

   Super::OnComponentDestroyed (bDestroyingHierarchy);
}

void URActiveStatusEffect::BeginPlay ()
{
   Super::BeginPlay ();
   Started ();
}

void URActiveStatusEffect::Started ()
{
   // --- Check Values
   if (!ensure (GetWorld ())) return;
   URStatusMgrComponent* StatusMgr_ = URUtil::GetComponent<URStatusMgrComponent> (GetOwner ());
   if (!ensure (StatusMgr_)) return;
   StatusMgr = StatusMgr_;

   IsRunning = true;

   Apply ();
   if (StatusMgr) StatusMgr->OnActiveEffectsUpdated.Broadcast ();
   OnStart.Broadcast ();
}

void URActiveStatusEffect::Stop ()
{
   UWorld* World = GetWorld ();
   if (!ensure (World)) return;
   if (TimerToEnd.IsValid ()) World->GetTimerManager ().ClearTimer (TimerToEnd);
   OnCancel.Broadcast ();
   Ended ();
}

void URActiveStatusEffect::Refresh_Implementation ()
{
   UWorld* World = GetWorld ();
   if (!ensure (World)) return;
   if (TimerToEnd.IsValid ()) World->GetTimerManager ().ClearTimer (TimerToEnd);

   if (StackMax > 1 && StackCurrent < StackMax) {
      // int StackLast = StackCurrent;
      StackCurrent = FMath::Clamp (StackCurrent + 1, 1, StackMax);

      // // --- For debug
      // const FRichCurve* StackToScaleData = StackToScale.GetRichCurveConst ();
      // if (!ensure (StackToScaleData))  return;
      // float StackLastScale    = StackToScaleData->Eval (StackLast);
      // float StackCurrentScale = StackToScaleData->Eval (StackCurrent);

      // R_LOG_PRINTF ("[%s] Effect stack increased [%d %.1f%] => [%d %.1f]",
      //    *GetName (),
      //    StackLast, StackLastScale * 100,
      //    StackCurrent, StackCurrentScale * 100);
   } else {
      // R_LOG_PRINTF ("[%s] Effect refreshed", *GetName ());
   }

   Apply ();
   OnRefresh.Broadcast ();
}

void URActiveStatusEffect::Apply ()
{
   UWorld* World = GetWorld ();
   if (!ensure (World)) return;
   StartTime = World->GetTimeSeconds ();

   if (R_IS_NET_ADMIN) {
      if (StatusMgr) {

         float StackScale = GetStackScale ();
         TArray<FRPassiveStatusEffect> CopyPassiveEffects = PassiveEffects;
         for (FRPassiveStatusEffect &ItPassiveEffect : CopyPassiveEffects) {
            ItPassiveEffect.Value = ItPassiveEffect.Value * StackScale;
         }
         StatusMgr->SetPassiveEffects (UIName, CopyPassiveEffects);
      }

      URDamageMgr *DamageMgr = URDamageMgr::GetInstance (this);
      if (DamageMgr) DamageMgr->ReportStatusEffect (this, Causer, GetOwner ());
   }

   if (Duration > 0) {
      World->GetTimerManager ().SetTimer (TimerToEnd, this, &URActiveStatusEffect::Ended, Duration, false);
   }
}

void URActiveStatusEffect::Ended ()
{
   IsRunning = false;

   if (StatusMgr) {
      if (R_IS_NET_ADMIN) StatusMgr->RmPassiveEffects (UIName);
      StatusMgr->OnActiveEffectsUpdated.Broadcast ();
   }
   OnEnd.Broadcast ();
   if (R_IS_NET_ADMIN) DestroyComponent ();
}

double URActiveStatusEffect::GetDurationLeft () const
{
   UWorld* World = GetWorld ();
   if (!ensure (World)) return 0;
   return FMath::Clamp (StartTime + Duration - World->GetTimeSeconds (), 0, Duration);
}

bool URActiveStatusEffect::GetIsRunning () const
{
   return IsRunning;
}

float URActiveStatusEffect::GetStackScale (int Stack) const
{
   if (Stack < 1) Stack = StackCurrent;
   const FRichCurve* StackToScaleData = StackToScale.GetRichCurveConst ();
   if (!ensure (StackToScaleData))  return 1;
   return StackToScaleData->Eval (Stack);
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

