// Copyright 2015-2023 Vagen Ayrapetyan

#include "RAbilityTypes.h"
#include "RAbilityMgrComponent.h"
#include "RWorldAbilityMgr.h"

#include "RUtilLib/RUtil.h"
#include "RUtilLib/RLog.h"
#include "RUtilLib/RCheck.h"

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
   if (URWorldAbilityMgr* WorldMgr = URWorldAbilityMgr::GetInstance (this)) {
      WorldMgr->ReportAddAbility (this);
   }
}
void URAbility::EndPlay (const EEndPlayReason::Type EndPlayReason)
{
   if (URWorldAbilityMgr* WorldMgr = URWorldAbilityMgr::GetInstance (this)) {
      WorldMgr->ReportRmAbility (this);
   }
   Super::EndPlay (EndPlayReason);
}

void URAbility::OnComponentCreated ()
{
   Super::OnComponentCreated ();

   if (!GetOwner ()->GetInstanceComponents ().Contains (this))
      GetOwner ()->AddInstanceComponent (this);

   if (URAbilityMgrComponent *Mgr = URUtil::GetComponent<URAbilityMgrComponent>(GetOwner ()))
      Mgr->OnAbilityListUpdated.Broadcast ();
}
void URAbility::OnComponentDestroyed (bool bDestroyingHierarchy)
{
   if (URAbilityMgrComponent *Mgr = URUtil::GetComponent<URAbilityMgrComponent>(GetOwner ()))
      Mgr->OnAbilityListUpdated.Broadcast ();

   if (GetOwner ()->GetInstanceComponents ().Contains (this))
      GetOwner ()->RemoveInstanceComponent (this);
   Super::OnComponentDestroyed (bDestroyingHierarchy);
}

void URAbility::SetIsEnabled (bool IsEnabled_)
{
   IsEnabled = IsEnabled_;
}

bool URAbility::GetIsEnabled () const
{
   return IsEnabled;
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
   AffectedType = ACharacter::StaticClass ();
}

void URAbility_Aura::BeginPlay ()
{
   Super::BeginPlay ();
   GetWorld ()->GetTimerManager ().SetTimer (TimerCheckRange,
                                             this, &URAbility_Aura::CheckRange,
                                             CheckRangeInterval, true);
}
void URAbility_Aura::CheckRange ()
{
   if (!AffectedType) return;

   FVector OwnerLocation = GetOwner ()->GetActorLocation ();

   TArray<AActor*> SearchResult;
   UGameplayStatics::GetAllActorsOfClass (this, AffectedType, SearchResult);

   TArray<AActor*> Result;
   for (AActor* ItActor : SearchResult) {
      if (!ItActor) continue;
      if (FVector::Distance (OwnerLocation, ItActor->GetActorLocation ()) > Range) continue;
      Result.Add (ItActor);
   }

   AffectedActors = Result;

   OnUpdated.Broadcast ();
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
         OnAbilityStatusUpdated.Broadcast ();
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
   OnAbilityUsed.Broadcast ();
   OnAbilityStatusUpdated.Broadcast ();
}

