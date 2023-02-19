// Copyright 2015-2023 Vagen Ayrapetyan

#include "RStatusEffect.h"
#include "RUtilLib/RLog.h"
#include "RUtilLib/RCheck.h"
#include "RStatusMgrComponent.h"

//=============================================================================
//                 Passive Effect
//=============================================================================

FRPassiveStatusEffect FRPassiveStatusEffect::operator + (const FRPassiveStatusEffect &obj) const
{
   FRPassiveStatusEffect res;
   res.Scale = Scale;
   res.Target = Target;
   res.Value = Value + obj.Value;
   return res;
}

//=============================================================================
//                 Active Effect
//=============================================================================

ARActiveStatusEffect::ARActiveStatusEffect ()
{
}

bool ARActiveStatusEffect::Apply (AActor *Causer_, AActor* Target_)
{
   if (isRunning) {
      R_LOG ("Already running")
      return false;
   }

   // -- Check values
   if (Causer_ == nullptr) {
      R_LOG ("Invalid Causer");
      return false;
   }
   if (Target_ == nullptr)  {
      R_LOG ("Invalid Target");
      return false;
   }
   if (!Target_->HasAuthority ())  {
      R_LOG ("Has no authority");
      return false;
   }

   UWorld* World = GetWorld ();
   if (World == nullptr)  {
      R_LOG ("No authority");
      return false;
   }

   URStatusMgrComponent* StatusMgr_ = URStatusMgrComponent::Get (Target_);
   if (StatusMgr_ == nullptr)   {
      R_LOG ("No Status mgr");
      return false;
   }

   // --- Set variables
   isRunning = true;
   Elapse    = FPlatformTime::Seconds () + Duration;
   Causer    = Causer_;
   Target    = Target_;
   StatusMgr = StatusMgr_;
   Started ();
   return true;
}

void ARActiveStatusEffect::Started ()
{
   AttachToActor (Target, FAttachmentTransformRules::SnapToTargetIncludingScale);
   if (StatusMgr && Duration) {
      StatusMgr->AddActiveEffect (this);
      StatusMgr->SetPassiveEffects (UIName, PassiveEffects);
   }
   BP_Started ();
   OnStart.Broadcast ();

   if (Duration > 0) {
      GetWorld ()->GetTimerManager ().SetTimer (TimerToEnd, this, &ARActiveStatusEffect::Ended, Duration, false);
   } else {
      Ended ();
   }
}

void ARActiveStatusEffect::Ended ()
{
   if (StatusMgr && Duration) {
      StatusMgr->RmActiveEffect (this);
      StatusMgr->RmPassiveEffects (UIName);
   }
   BP_Ended ();
   OnEnd.Broadcast ();
   Destroy ();
}

void ARActiveStatusEffect::Cancel ()
{
   TimerToEnd.Invalidate ();
   Ended ();
}

double ARActiveStatusEffect::GetDurationLeft () const
{
   if (!Duration) return 0;
   if (!Elapse)   return 0;
   double dt = Elapse - FPlatformTime::Seconds ();
   if (dt < 0) dt = 0;
   return dt;
}

//=============================================================================
//                 Effect Library
//=============================================================================

bool URStatusEffectUtilLibrary::SetStatusEffect_Passive (AActor *Target, const FString &Tag, const TArray<FRPassiveStatusEffect> &Effects)
{
   // --- Get Status Mgr
   if (Target == nullptr)        return false;
   if (!Target->HasAuthority ()) return false;

   URStatusMgrComponent* StatusMgr = URStatusMgrComponent::Get (Target);
   if (!StatusMgr) return false;

   // --- Action
   return StatusMgr->SetPassiveEffects (Tag, Effects);
}

bool URStatusEffectUtilLibrary::RmStatusEffect_Passive (AActor *Target, const FString &Tag)
{
   // --- Get Status Mgr
   if (Target == nullptr)        return false;
   if (!Target->HasAuthority ()) return false;

   URStatusMgrComponent* StatusMgr = URStatusMgrComponent::Get (Target);
   if (!StatusMgr) return false;

   // --- Action
   return StatusMgr->RmPassiveEffects (Tag);
}

bool URStatusEffectUtilLibrary::ApplyStatusEffect_Active (AActor* Causer, AActor *Target, const TSubclassOf<ARActiveStatusEffect> Effect)
{
   // --- Get Status Mgr
   if (Causer == nullptr)        return false;
   if (Target == nullptr)        return false;
   if (Effect == nullptr)        return false;
   if (!Causer->HasAuthority ()) return false;
   UWorld* World = Target->GetWorld ();
   if (!World) return false;

   // --- Action
   ARActiveStatusEffect* NewEffect = World->SpawnActor<ARActiveStatusEffect>(Effect);
   if (!NewEffect) return false;
   return NewEffect->Apply (Causer, Target);
}

