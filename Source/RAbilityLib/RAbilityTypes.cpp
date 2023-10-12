// Copyright 2015-2023 Vagen Ayrapetyan

#include "RAbilityTypes.h"
#include "RAbilityMgrComponent.h"
#include "RWorldAbilityMgr.h"

#include "RUtilLib/RUtil.h"
#include "RUtilLib/RLog.h"
#include "RUtilLib/RCheck.h"
#include "RUtilLib/RWorldAssetMgr.h"

// ============================================================================
//                   AbilityInfo
// ============================================================================

bool URAbilityInfolLibrary::AbilityInfo_IsValid (const FRAbilityInfo& AbilityInfo)
{
   return AbilityInfo.IsValid ();
}

//=============================================================================
//                 Passsive Ability
//=============================================================================

URAbility::URAbility ()
{
   SetIsReplicatedByDefault (true);
}

void URAbility::BeginPlay ()
{
   Super::BeginPlay ();

   if (GetWorld () && !GetOwner ()->GetInstanceComponents ().Contains (this))
      GetOwner ()->AddInstanceComponent (this);

   if (URAbilityMgrComponent *Mgr = URUtil::GetComponent<URAbilityMgrComponent>(GetOwner ()))
      Mgr->ReportAbilityListUpdated ();

   PullAbilityInfo ();
}

void URAbility::EndPlay (const EEndPlayReason::Type EndPlayReason)
{
   if (GetWorld () && GetOwner ()->GetInstanceComponents ().Contains (this))
      GetOwner ()->RemoveInstanceComponent (this);

   if (URAbilityMgrComponent* Mgr = URUtil::GetComponent<URAbilityMgrComponent>(GetOwner ()))
      Mgr->ReportAbilityListUpdated ();

   if (URWorldAbilityMgr* WorldMgr = URWorldAbilityMgr::GetInstance (this)) {
      WorldMgr->ReportRmAbility (this);
   }
   Super::EndPlay (EndPlayReason);
}

void URAbility::PullAbilityInfo ()
{
   if (URWorldAbilityMgr* WorldMgr = URWorldAbilityMgr::GetInstance (this)) {
      WorldMgr->ReportAddAbility (this);
      AbilityInfo = WorldMgr->GetAbilityInfo_Object (this);
   }

   if (!AbilityInfo.IsValid ()) {
      FTimerHandle RepeatTimer;
      GetWorld ()->GetTimerManager ().SetTimer (RepeatTimer,
                                                this, &URAbility_Aura::PullAbilityInfo,
                                                1);
   }
}

void URAbility::SetIsEnabled (bool IsEnabled_)
{
   IsEnabled = IsEnabled_;
}

bool URAbility::GetIsEnabled () const
{
   return IsEnabled;
}

FRAbilityInfo URAbility::GetAbilityInfo () const
{
   return AbilityInfo;
}

FRUIDescription URAbility::GetDescription_Implementation ()
{
   return AbilityInfo.Description;
}


//=============================================================================
//                 Passsive Ability
//=============================================================================

URAbility_Passive::URAbility_Passive ()
{
   PrimaryComponentTick.bCanEverTick = false;
   PrimaryComponentTick.bStartWithTickEnabled = false;
}

//=============================================================================
//                 Passsive Ability
//=============================================================================

URAbility_Aura::URAbility_Aura ()
{
   TargetClass = AActor::StaticClass ();
}

void URAbility_Aura::BeginPlay ()
{
   Super::BeginPlay ();
   LoadTargetClass ();
}

void URAbility_Aura::EndPlay (const EEndPlayReason::Type EndPlayReason)
{
   SetCheckRangeActive (false);
   Super::EndPlay (EndPlayReason);
}

void URAbility_Aura::LoadTargetClass ()
{
   if (ensure (!TargetClass.IsNull ())) {
      URWorldAssetMgr* AssetMgr = URWorldAssetMgr::GetInstance (this);
      if (AssetMgr) {
         TargetClassLoadHandle = AssetMgr->StreamableManager.RequestAsyncLoad (TargetClass.GetUniqueID (),
            [this] () {
               if (!TargetClassLoadHandle.IsValid ()) return;
               if (TargetClassLoadHandle->HasLoadCompleted ()) {
                  if (UObject* Obj = TargetClassLoadHandle->GetLoadedAsset ()) {
                     if (UClass* LoadedClass = Cast<UClass> (Obj)) {
                        TargetClassLoaded = LoadedClass;
                     }
                  }
               }

               // Release data from memory
               TargetClassLoadHandle->ReleaseHandle ();
               TargetClassLoadHandle.Reset ();

               SetCheckRangeActive (true);
            });
      } else {
         FTimerHandle RetryHandle;
         GetWorld ()->GetTimerManager ().SetTimer (RetryHandle,
                                                   this,
                                                   &URAbility_Aura::LoadTargetClass,
                                                   1,
                                                   false);
      }
   }
}

void URAbility_Aura::SetCheckRangeActive (bool enable)
{
   if (enable) {
      if (!TimerCheckRange.IsValid () && TargetClassLoaded)
         GetWorld ()->GetTimerManager ().SetTimer (TimerCheckRange,
                                                   this,
                                                   &URAbility_Aura::CheckRange,
                                                   CheckRangeInterval,
                                                   true,
                                                   0);

   } else {
      if (TimerCheckRange.IsValid ())
         GetWorld ()->GetTimerManager ().ClearTimer (TimerCheckRange);
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

//=============================================================================
//                 Active Ability
//=============================================================================

URAbility_Active::URAbility_Active ()
{
   PrimaryComponentTick.bCanEverTick = true;
   PrimaryComponentTick.bStartWithTickEnabled = true;
   PrimaryComponentTick.TickInterval = 0.5f;
}

void URAbility_Active::TickComponent (float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction *ThisTickFunction)
{
   Super::TickComponent (DeltaTime, TickType, ThisTickFunction);

   if (!IsUseable) {
      // Update cooldown left
      UWorld* World = GetWorld ();
      if (!ensure (World)) return;
      UseCooldownLeft = FMath::Clamp (UseLastTime + Cooldown - World->GetTimeSeconds (), 0, Cooldown);

      // Can be used
      if (UseCooldownLeft == 0 && GetIsEnabled ()) {
         IsUseable = true;
         if (R_IS_VALID_WORLD && OnAbilityStatusUpdated.IsBound ()) OnAbilityStatusUpdated.Broadcast ();
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
   UWorld* World = GetWorld ();
   if (!ensure (World)) return;

   // Update local state
   UseLastTime     = World->GetTimeSeconds ();
   UseCooldownLeft = Cooldown;
   IsUseable       = false;

   if (URWorldAbilityMgr* WorldMgr = URWorldAbilityMgr::GetInstance (this)) {
      WorldMgr->ReportUseAbility (this);
   }

   // Broadcast event
   if (R_IS_VALID_WORLD) {
      if (OnAbilityUsed.IsBound ())          OnAbilityUsed.Broadcast ();
      if (OnAbilityStatusUpdated.IsBound ()) OnAbilityStatusUpdated.Broadcast ();
   }
}

