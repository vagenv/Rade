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

void ARActiveStatusEffect::Start (AActor * Causer_, AActor* Target_)
{
   if (isRunning) return;

   isRunning = true;
   Causer = Causer_;
   Target = Target_;
   Started ();
}

void ARActiveStatusEffect::Started ()
{
   BP_Started ();
   GetWorld ()->GetTimerManager ().SetTimer (TimerToEnd, this, &ARActiveStatusEffect::Ended, Duration, false);
   OnStart.Broadcast ();
}

void ARActiveStatusEffect::Ended ()
{
   BP_Ended ();
   OnEnd.Broadcast ();

   Destroy ();
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
   return StatusMgr->SetEffects (Tag, Effects);
}

bool URStatusEffectUtilLibrary::RmStatusEffect_Passive (AActor *Target, const FString &Tag)
{
   // --- Get Status Mgr
   if (Target == nullptr)        return false;
   if (!Target->HasAuthority ()) return false;

   URStatusMgrComponent* StatusMgr = URStatusMgrComponent::Get (Target);
   if (!StatusMgr) return false;

   // --- Action
   return StatusMgr->RmEffects (Tag);
}

bool URStatusEffectUtilLibrary::ApplyStatusEffect_Active (AActor* Causer, AActor *Target, const TSubclassOf<ARActiveStatusEffect> Effect)
{
   // --- Get Status Mgr
   if (Causer == nullptr)        return false;
   if (Target == nullptr)        return false;
   if (Effect == nullptr)        return false;
   if (!Causer->HasAuthority ()) return false;

   URStatusMgrComponent* StatusMgr = URStatusMgrComponent::Get (Target);
   if (!StatusMgr) return false;

   // --- Action
   UWorld* World = Target->GetWorld ();
   if (!World) return false;

   FVector  Loc = Target->GetActorLocation ();
   FRotator Rot = Target->GetActorRotation ();
   ARActiveStatusEffect* NewEffect = World->SpawnActor<ARActiveStatusEffect>(Effect, Loc, Rot);
   if (!NewEffect) return false;

   NewEffect->AttachToActor (Target, FAttachmentTransformRules::SnapToTargetIncludingScale);
   NewEffect->Start (Causer, Target);
   return true;
}

