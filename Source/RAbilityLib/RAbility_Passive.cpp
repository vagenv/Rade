// Copyright 2015-2023 Vagen Ayrapetyan

#include "RAbility_Passive.h"

#include "RUtilLib/RUtil.h"
#include "RUtilLib/RLog.h"
#include "RUtilLib/RCheck.h"
#include "RUtilLib/RTimer.h"
#include "RUtilLib/RWorldAssetMgr.h"

//=============================================================================
//                 Passsive Ability
//=============================================================================

URAbility_Passive::URAbility_Passive ()
{
   PrimaryComponentTick.bCanEverTick = false;
   PrimaryComponentTick.bStartWithTickEnabled = false;
}

//=============================================================================
//                 Aura Ability
//=============================================================================

URAbility_Aura::URAbility_Aura ()
{
   TargetClass = AActor::StaticClass ();
}

void URAbility_Aura::BeginPlay ()
{
   Super::BeginPlay ();

   if (ensure (!TargetClass.IsNull ())) {
      URWorldAssetMgr::LoadAsync (TargetClass.GetUniqueID (), this, [this] (UObject* LoadedContent) {
         if (UClass* LoadedClass = Cast<UClass> (LoadedContent)) {
            TargetClassLoaded = LoadedClass;
         }
         SetCheckRangeActive (true);
      });
   }
}

void URAbility_Aura::EndPlay (const EEndPlayReason::Type EndPlayReason)
{
   SetCheckRangeActive (false);
   Super::EndPlay (EndPlayReason);
}

void URAbility_Aura::SetCheckRangeActive (bool Enable)
{
   if (Enable) {
      if (TargetClassLoaded)
        RTIMER_START (CheckRangeHandle,
                      this, &URAbility_Aura::CheckRange,
                      CheckRangeInterval,
                      true);
      // Instantly call
      CheckRange ();

   } else {
      RTIMER_STOP (CheckRangeHandle, this);
   }
}

void URAbility_Aura::CheckRange ()
{
   if (!TargetClassLoaded) return;

   FVector OwnerLocation = GetOwner ()->GetActorLocation ();
   TArray<AActor*> SearchResult;
   UGameplayStatics::GetAllActorsOfClass (this, TargetClassLoaded, SearchResult);

   TArray<TWeakObjectPtr<AActor> > Result;
   for (AActor* ItActor : SearchResult) {
      if (!IsValid (ItActor)) continue;
      if (FVector::Distance (OwnerLocation, ItActor->GetActorLocation ()) > Range) continue;

      Result.Add (ItActor);
   }

   AffectedActors = Result;
   if (R_IS_VALID_WORLD && OnCheckRange.IsBound ()) OnCheckRange.Broadcast ();
}

const TArray<AActor* > URAbility_Aura::GetAffectedActors() const
{
   TArray<AActor*> Result;
   for (const auto &ItActor : AffectedActors) {
      if (ItActor.IsValid ()) Result.Add (ItActor.Get ());
   }
   return Result;
}

