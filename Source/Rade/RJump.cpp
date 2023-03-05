// Copyright 2015-2023 Vagen Ayrapetyan

#include "RJump.h"
#include "RUtilLib/RUtil.h"
#include "RUtilLib/RLog.h"
#include "RStatusLib/RStatusMgrComponent.h"

//=============================================================================
//             Jump
//=============================================================================

void URAbility_Jump::Use ()
{
   if (!CanUse ()) return;

   ACharacter *Character = Cast<ACharacter> (GetOwner ());
   if (!ensure (Character)) return;
   URStatusMgrComponent* StatusMgr = URUtil::GetComponent<URStatusMgrComponent> (GetOwner ());
   if (!ensure (StatusMgr)) return;

   StatusMgr->UseStamina (StaminaCost);
   Character->Jump ();
}

bool URAbility_Jump::CanUse ()
{
   ACharacter *Character = Cast<ACharacter> (GetOwner ());
   if (!ensure (Character)) return false;

   UPawnMovementComponent *MoveComponent = Character->GetMovementComponent ();
   if (!ensure (MoveComponent)) return false;

   if (!MoveComponent->IsMovingOnGround ()) return false;

   URStatusMgrComponent* StatusMgr = URUtil::GetComponent<URStatusMgrComponent> (GetOwner ());
   if (!ensure (StatusMgr)) return false;

   FRStatusValue Stamina = StatusMgr->GetStamina ();
   return Stamina.Current > StaminaCost;
}

//=============================================================================
//             Double Jump
//=============================================================================

URAbility_DoubleJump::URAbility_DoubleJump ()
{
   // Default damage Curve
   FRichCurve* AgiToJumpPowerData = AgiToJumpPower.GetRichCurve ();

   // Resistance to reduction percent
   AgiToJumpPowerData->AddKey (   0,  700);
   AgiToJumpPowerData->AddKey (  10,  900);
   AgiToJumpPowerData->AddKey (  50, 1200);
   AgiToJumpPowerData->AddKey ( 100, 1500);
}

void URAbility_DoubleJump::Use ()
{
   if (!CanUse ()) return;

   ACharacter *Character = Cast<ACharacter> (GetOwner ());
   if (!ensure (Character)) return;
   URStatusMgrComponent* StatusMgr = URUtil::GetComponent<URStatusMgrComponent> (GetOwner ());
   if (!ensure (StatusMgr)) return;
   UPawnMovementComponent *MoveComponent = Character->GetMovementComponent ();
   if (!ensure (MoveComponent)) return;

   const FRichCurve* AgiToJumpPowerData = AgiToJumpPower.GetRichCurveConst ();
   if (!ensure (AgiToJumpPowerData)) return;

   FRStatusValue Mana = StatusMgr->GetMana ();
   // Apply power
   float JumpVelocity = AgiToJumpPowerData->Eval (StatusMgr->GetCoreStats_Total ().AGI);

   //R_LOG_PRINTF ("Jump power : %f", JumpVelocity);
   // TODO: Get Move Vector, Normalize.
   MoveComponent->Velocity.Z += JumpVelocity;

   StatusMgr->UseMana (ManaCost);
}

bool URAbility_DoubleJump::CanUse ()
{
   ACharacter *Character = Cast<ACharacter> (GetOwner ());
   if (!ensure (Character)) return false;

   UPawnMovementComponent *MoveComponent = Character->GetMovementComponent ();
   if (!ensure (MoveComponent)) return false;

   if (MoveComponent->IsMovingOnGround ()) return false;

   URStatusMgrComponent* StatusMgr = URUtil::GetComponent<URStatusMgrComponent> (GetOwner ());
   if (!ensure (StatusMgr)) return false;

   FRStatusValue Mana = StatusMgr->GetMana ();
   return Mana.Current > ManaCost;
}

