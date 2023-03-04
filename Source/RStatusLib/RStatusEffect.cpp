// Copyright 2015-2023 Vagen Ayrapetyan

#include "RStatusEffect.h"
#include "RUtilLib/RUtil.h"
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
   // --- Check Values
   if (!ensure (Causer_))                  return false;
   if (!ensure (Target_))                  return false;
   if (!ensure (!isRunning))               return false;
   if (!ensure (Causer_->HasAuthority ())) return false;
   UWorld* World = GetWorld ();
   if (!ensure (World))                    return false;
   URStatusMgrComponent* StatusMgr_ = URUtil::GetComponent<URStatusMgrComponent> (GetOwner ());
   if (!ensure (StatusMgr_))               return false;


   // --- Check Refresh/Stacking
   TArray<ARActiveStatusEffect*> CurrentEffects = StatusMgr_->GetActiveEffects ();
   for (ARActiveStatusEffect* ItActiveEffect : CurrentEffects) {
      if (ItActiveEffect->GetClass () == GetClass ()) {
         if (StackMax > 1) {
            StackCurrent = FMath::Clamp (ItActiveEffect->StackCurrent + 1., 1., StackMax);
            for (FRPassiveStatusEffect &ItPassiveEffect : PassiveEffects) {
               ItPassiveEffect.Value *= StackCurrent;
            }
         }
         ItActiveEffect->Cancel ();
         break;
      }
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
   BP_Canceled ();
   OnCancel.Broadcast ();
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
   // --- Check Values
   if (!ensure (Target))                  return false;
   if (!ensure (Target->HasAuthority ())) return false;
   if (!ensure (!Tag.IsEmpty ()))         return false;
   URStatusMgrComponent* StatusMgr = URUtil::GetComponent<URStatusMgrComponent> (Target);
   if (!ensure (StatusMgr))               return false;

   // --- Action
   return StatusMgr->SetPassiveEffects (Tag, Effects);
}

bool URStatusEffectUtilLibrary::RmStatusEffect_Passive (AActor *Target, const FString &Tag)
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

bool URStatusEffectUtilLibrary::ApplyStatusEffect_Active (AActor* Causer, AActor *Target, const TSubclassOf<ARActiveStatusEffect> Effect)
{
   // --- Check Values
   if (!ensure (Causer))                  return false;
   if (!ensure (Causer->HasAuthority ())) return false;
   if (!ensure (Target))                  return false;
   UWorld* World = Target->GetWorld ();
   if (!World)                            return false;
   if (!ensure (Effect))                  return false;
   URStatusMgrComponent* StatusMgr = URUtil::GetComponent<URStatusMgrComponent> (Target);
   if (!ensure (StatusMgr))               return false;

   // --- Action
   ARActiveStatusEffect* NewEffect = World->SpawnActor<ARActiveStatusEffect>(Effect);
   if (!ensure (NewEffect)) return false;
   return NewEffect->Apply (Causer, Target);
}

