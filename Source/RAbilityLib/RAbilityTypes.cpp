// Copyright 2015-2023 Vagen Ayrapetyan

#include "RAbilityTypes.h"

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
   PrimaryComponentTick.bCanEverTick = true;
   PrimaryComponentTick.bStartWithTickEnabled = true;
   PrimaryComponentTick.TickInterval = 1;

   AffectedType = ACharacter::StaticClass ();
}

void URAbility_Aura::BeginPlay ()
{
   Super::BeginPlay ();

   if (!R_IS_NET_ADMIN) {
      SetComponentTickEnabled (false);
   }
}

void URAbility_Aura::TickComponent (float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction *ThisTickFunction)
{
   Super::TickComponent (DeltaTime, TickType, ThisTickFunction);
   CheckRange ();
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
   R_RETURN_IF_NOT_ADMIN;
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

void URAbility_Active::Use_Server_Implementation ()
{
   Use ();
}