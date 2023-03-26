// Copyright 2015-2023 Vagen Ayrapetyan

#include "RExperienceMgrComponent.h"
#include "RWorldExperienceMgr.h"
#include "RUtilLib/RCheck.h"
#include "RUtilLib/RLog.h"

#include "Net/UnrealNetwork.h"

URExperienceMgrComponent::URExperienceMgrComponent ()
{
   SetIsReplicatedByDefault (true);
}

// Replication
void URExperienceMgrComponent::GetLifetimeReplicatedProps (TArray<FLifetimeProperty> &OutLifetimeProps) const
{
   Super::GetLifetimeReplicatedProps (OutLifetimeProps);
   DOREPLIFETIME (URExperienceMgrComponent, ExperiencePoints);
}

void URExperienceMgrComponent::OnRep_Exp ()
{
   if (R_IS_VALID_WORLD) OnExperienceChange.Broadcast ();
   CheckLevel ();
}

void URExperienceMgrComponent::BeginPlay ()
{
   Super::BeginPlay ();
   GlobalMgr = URWorldExperienceMgr::GetInstance (this);
}

void URExperienceMgrComponent::EndPlay (const EEndPlayReason::Type EndPlayReason)
{
   Super::EndPlay (EndPlayReason);
}

void URExperienceMgrComponent::AddExperiencePoints (int64 ExpPoint)
{
   if (ExpPoint <= 0) return;

   // R_LOG_PRINTF ("Exp Changed [%lld -> %lld]",
   //       ExperiencePoints,
   //       ExperiencePoints + ExpPoint);

   ExperiencePoints += ExpPoint;
   if (R_IS_VALID_WORLD) OnExperienceChange.Broadcast ();
   CheckLevel ();
}

int64 URExperienceMgrComponent::GetExperiencePoints () const
{
   return ExperiencePoints;
}

void URExperienceMgrComponent::CheckLevel ()
{
   if (!GlobalMgr) return;

   int NewLevel = GlobalMgr->ExperienceToLevel (GetExperiencePoints ());
   // R_LOG_PRINTF ("Leveled change [%d => %d]", CurrentLevel, NewLevel);
   int dtLevel = NewLevel - GetCurrentLevel ();
   if (dtLevel <= 0) return;

   for (int i = 0; i < dtLevel; i++) {
      LeveledUp ();
   }

   // // Provide delta left
   // int64 ExpNext = GlobalMgr->LevelToExperience (GetCurrentLevel () + 1);
   // int64 dtExp  =  ExpNext - GetExperiencePoints ();

   // R_LOG_PRINTF ("Exp Current [%lld] Next: [%lld]",
   //    GetExperiencePoints (),
   //    ExpNext);
}

int URExperienceMgrComponent::GetCurrentLevel () const
{
   return CurrentLevel;
}

void URExperienceMgrComponent::LeveledUp ()
{
   // R_LOG_PRINTF ("Leveled up!!! [%d => %d]", CurrentLevel, CurrentLevel + 1);
   CurrentLevel++;
   if (R_IS_VALID_WORLD) OnLevelUp.Broadcast ();
}

