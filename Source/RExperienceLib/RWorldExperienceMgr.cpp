// Copyright 2015-2023 Vagen Ayrapetyan

#include "RWorldExperienceMgr.h"
#include "RExperienceMgrComponent.h"
#include "RUtilLib/RLog.h"
#include "RUtilLib/RUtil.h"
#include "RUtilLib/RCheck.h"
#include "RDamageLib/RWorldDamageMgr.h"

//=============================================================================
//                   Static calls
//=============================================================================

URWorldExperienceMgr* URWorldExperienceMgr::GetInstance (UObject* WorldContextObject)
{
   if (!ensure (WorldContextObject)) return nullptr;

   UWorld *World = WorldContextObject->GetWorld ();
   if (!ensure (World)) return nullptr;

   AGameStateBase *GameState = World->GetGameState ();
   if (!ensure (GameState)) return nullptr;

   return GameState->FindComponentByClass<URWorldExperienceMgr>();
}

//=============================================================================
//                   Member calls
//=============================================================================

URWorldExperienceMgr::URWorldExperienceMgr ()
{
   // --- For presentation
   // FRichCurve* LevelToExperienceCurveData = LevelToExperienceCurve.GetRichCurve ();
   // LevelToExperienceCurveData->AddKey (0, 0);
   // for (int i = 1; i < 100; i++) {
   //    LevelToExperienceCurveData->AddKey (i, LevelToExperience (i));
   // }
   // // LevelToExperienceCurveData->AddKey (  20, LevelToExperience (  20));
   // // LevelToExperienceCurveData->AddKey ( 100, LevelToExperience ( 100));
   // // LevelToExperienceCurveData->AddKey (1000, LevelToExperience (1000));
}

int URWorldExperienceMgr::ExperienceToLevel (int64 ExpPoints) const
{
   if (ExpPoints <= 0) return 0;
   double Exponent = 2.5;
   double Base     = 0.75;
   return floor (pow (ExpPoints / Base, 1. / Exponent));
}

int64 URWorldExperienceMgr::LevelToExperience (int Level) const
{
   if (Level <= 0) return 0;
   if (true) {
      //   1 ->      1
      //   2 ->      5
      //   3 ->     12
      //   4 ->     24
      //   5 ->     42
      //  10 ->    238
      //  20 ->   1342
      //  50 ->  13259
      // 100 ->  75000
      // 200 -> 424265
      double Exponent = 2.5;
      double Base     = 0.75;
      return ceil (Base * pow (Level, Exponent));
   }

   if (false) {
      //   1 ->       1
      //   2 ->       6
      //   3 ->      20
      //   4 ->      48
      //   5 ->      94
      //  10 ->     750
      //  20 ->    6000
      //  50 ->   93750
      // 100 ->  750000
      // 200 -> 6000000

      double Exponent = 3;
      double Base     = 0.75f;
      return FMath::RoundToInt64 (Base * FMath::Pow (Level, Exponent));
   }

   if (false) {
      //   1 ->        1
      //   2 ->        6
      //   3 ->       23
      //   4 ->       64
      //   5 ->      140
      //  10 ->     1581
      //  20 ->    17889
      //  50 ->   441942
      // 100 ->  5000000
      // 200 -> 56568542

      double Exponent = 3.5;
      double Base     = 0.5f;
      return FMath::RoundToInt64 (Base * FMath::Pow (Level, Exponent));
   }

   if (false) {
      //   1 ->      100
      //   2 ->      283
      //   3 ->      520
      //   4 ->      800
      //   5 ->     1118
      //  10 ->     3162
      //  20 ->     8944
      //  50 ->    35355
      // 100 ->   100000
      // 200 ->   282843

      double Exponent = 1.5f;
      double Base     = 100;
      return FMath::RoundToInt64 (Base * FMath::Pow (Level, Exponent));
   }

   if (false) {
      //   1 ->      204
      //   2 ->      816
      //   3 ->     1837
      //   4 ->     3265
      //   5 ->     5102
      //  10 ->    20408
      //  20 ->    81633
      //  50 ->   510204
      // 100 ->  2040816
      // 200 ->  8163265

      double Exponent = 2;
      double Base     = 0.07f;
      return FMath::RoundToInt64 (FMath::Pow (Level / Base, Exponent));
   }
}

void URWorldExperienceMgr::BeginPlay ()
{
   Super::BeginPlay ();

   if (R_IS_NET_ADMIN) {
      // --- Subscribe to Damage and Death Events
      if (URWorldDamageMgr *DamageMgr = URUtil::GetComponent<URWorldDamageMgr> (GetOwner ())) {
         DamageMgr->OnAnyRDamage.AddDynamic (this, &URWorldExperienceMgr::OnDamage);
         DamageMgr->OnDeath.AddDynamic (this, &URWorldExperienceMgr::OnDeath);
      }

      // --- Parse Table and create Map for fast search
      if (EnemyExpTable) {
         FString ContextString;
         TArray<FName> RowNames = EnemyExpTable->GetRowNames ();
         for (const FName& ItRowName : RowNames) {
            FREnemyExp* ItRow = EnemyExpTable->FindRow<FREnemyExp> (ItRowName, ContextString);
            if (ItRow && ItRow->Target) {
               MapEnemyExp.Add (ItRow->Target, *ItRow);
            }
         }
      }
   }
}

void URWorldExperienceMgr::OnDamage (AActor*             Victim,
                                     float               Amount,
                                     const URDamageType* Type,
                                     AActor*             Causer)
{
   R_RETURN_IF_NOT_ADMIN;
   if (!ensure (Victim)) return;
   if (!ensure (Type))   return;
   if (!ensure (Causer)) return;

   URExperienceMgrComponent *ExpMgr = URUtil::GetComponent<URExperienceMgrComponent> (Causer);
   if (!ExpMgr) return;

   if (!MapEnemyExp.Contains (Victim->GetClass ())) return;
   ExpMgr->AddExperiencePoints (MapEnemyExp[Victim->GetClass ()].PerDamage * Amount);
}

void URWorldExperienceMgr::OnDeath (AActor* Victim,
                                    AActor* Causer,
                                    const URDamageType* Type)
{
   R_RETURN_IF_NOT_ADMIN;
   if (!ensure (Victim)) return;
   if (!ensure (Causer)) return;
   if (!ensure (Type))   return;

   URExperienceMgrComponent *ExpMgr = URUtil::GetComponent<URExperienceMgrComponent> (Causer);
   if (!ExpMgr) return;

   if (!MapEnemyExp.Contains (Victim->GetClass ())) return;
   ExpMgr->AddExperiencePoints (MapEnemyExp[Victim->GetClass ()].PerDeath);
}

