// Copyright 2015-2023 Vagen Ayrapetyan

#include "RJump.h"
#include "RUtilLib/RUtil.h"
#include "RUtilLib/RLog.h"
#include "RStatusLib/RStatusMgrComponent.h"

//=============================================================================
//             Jump
//=============================================================================

URAbility_Jump::URAbility_Jump()
{
   Cooldown = 1;
   UIName = "Jump";
}

void URAbility_Jump::BeginPlay ()
{
   Super::BeginPlay ();

   Character = Cast<ACharacter> (GetOwner ());
   if (!ensure (Character)) return;

   MoveComponent = Character->GetMovementComponent ();
   if (!ensure (MoveComponent)) return;

   StatusMgr = URUtil::GetComponent<URStatusMgrComponent> (GetOwner ());
   if (!ensure (StatusMgr)) return;
}

void URAbility_Jump::TickComponent (float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction *ThisTickFunction)
{
   Super::TickComponent (DeltaTime, TickType, ThisTickFunction);

   if (MoveComponent) {
      OnGround = MoveComponent->IsMovingOnGround ();
   }
}

void URAbility_Jump::Use ()
{
   if (!CanUse ())          return;
   if (!ensure (Character)) return;
   if (!ensure (StatusMgr)) return;

   StatusMgr->UseStamina (StaminaCost);
   Character->Jump ();
   Super::Use ();
}

bool URAbility_Jump::CanUse () const
{
   if (!Super::CanUse ())   return false;
   if (!OnGround)           return false;
   if (!ensure (StatusMgr)) return false;

   FRStatusValue Stamina = StatusMgr->GetStamina ();
   return Stamina.Current > StaminaCost;
}

//=============================================================================
//             Double Jump
//=============================================================================

URAbility_DoubleJump::URAbility_DoubleJump ()
{
   UIName = "Double Jump";

   // Default damage Curve
   FRichCurve* AgiToJumpPowerData = AgiToJumpPower.GetRichCurve ();

   // Resistance to reduction percent
   AgiToJumpPowerData->AddKey (   0,  700);
   AgiToJumpPowerData->AddKey (  10,  900);
   AgiToJumpPowerData->AddKey (  50, 1200);
   AgiToJumpPowerData->AddKey ( 100, 1500);
}

void URAbility_DoubleJump::BeginPlay ()
{
   Super::BeginPlay ();

   Character = Cast<ACharacter> (GetOwner ());
   if (!ensure (Character)) return;

   MoveComponent = Character->GetMovementComponent ();
   if (!ensure (MoveComponent)) return;

   StatusMgr = URUtil::GetComponent<URStatusMgrComponent> (GetOwner ());
   if (!ensure (StatusMgr)) return;
}

void URAbility_DoubleJump::TickComponent (float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction *ThisTickFunction)
{
   Super::TickComponent (DeltaTime, TickType, ThisTickFunction);

   if (MoveComponent) {
      OnGround = MoveComponent->IsMovingOnGround ();
      if (OnGround) Landed = true;
   }
}

void URAbility_DoubleJump::Use ()
{
   if (!CanUse ())              return;
   if (!ensure (Character))     return;
   if (!ensure (StatusMgr))     return;
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
   Super::Use ();
   Landed = false;
}

bool URAbility_DoubleJump::CanUse () const
{
   if (!Super::CanUse ())   return false;
   if (OnGround)            return false;
   if (!Landed)             return false;
   if (!ensure (StatusMgr)) return false;

   FRStatusValue Mana = StatusMgr->GetMana ();
   return Mana.Current > ManaCost;
}

