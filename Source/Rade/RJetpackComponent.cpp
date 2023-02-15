// Copyright 2015-2023 Vagen Ayrapetyan

#include "RJetpackComponent.h"
#include "RUtilLib/RLog.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/Character.h"
#include "Math/UnrealMathUtility.h"
#include "RStatusLib/RStatusMgrComponent.h"

URJetpackComponent::URJetpackComponent ()
{
   PrimaryComponentTick.bCanEverTick = true;
   PrimaryComponentTick.bStartWithTickEnabled = true;

   // Default damage Curve
   FRichCurve* AgiToJumpPowerData = AgiToJumpPower.GetRichCurve ();

   // Resistance to reduction percent
   AgiToJumpPowerData->AddKey (   0,  700);
   AgiToJumpPowerData->AddKey (  10,  900);
   AgiToJumpPowerData->AddKey (  50, 1200);
   AgiToJumpPowerData->AddKey ( 100, 1500);
}

void URJetpackComponent::BeginPlay()
{
   Super::BeginPlay ();
}

bool URJetpackComponent::CanUse () const
{
   UCharacterMovementComponent* MovementComponent = GetMovementComponent ();
   if (!MovementComponent) return false;
   if (MovementComponent->IsMovingOnGround ()) return false;

   URStatusMgrComponent* StatusMgr = GetStatusMgr ();
   if (!StatusMgr) return false;

   float MinUseablePercent = 40;
   FRStatusValue Stamina = StatusMgr->GetStamina ();
   float CurrentChargePercent = Stamina.Current * 100 / Stamina.Max;

   return (CurrentChargePercent > MinUseablePercent);
}

void URJetpackComponent::Use_Implementation ()
{
   if (!CanUse ()) return;
   UCharacterMovementComponent* MovementComponent = GetMovementComponent ();
   if (!MovementComponent) return;
   URStatusMgrComponent* StatusMgr = GetStatusMgr ();
   if (!StatusMgr) return;

   const FRichCurve* AgiToJumpPowerData = AgiToJumpPower.GetRichCurveConst ();
   if (!ensure (AgiToJumpPowerData)) return;

   FRStatusValue Stamina = StatusMgr->GetStamina ();
   float CurrentChargePercent = Stamina.Current / Stamina.Max;
   // Apply power
   float JumpVelocity = AgiToJumpPowerData->Eval (StatusMgr->GetCoreStats_Total ().AGI) * CurrentChargePercent;

   R_LOG_PRINTF ("Jump power : %f", JumpVelocity);
   MovementComponent->Velocity.Z += JumpVelocity;
   StatusMgr->UseStamina (Stamina.Max);
}

URStatusMgrComponent* URJetpackComponent::GetStatusMgr () const
{
   URStatusMgrComponent* StatusMgr = nullptr;
   {
      TArray<URStatusMgrComponent*> StatusMgrList;
      GetOwner ()->GetComponents (StatusMgrList);
      if (StatusMgrList.Num ()) StatusMgr = StatusMgrList[0];
   }
   return StatusMgr;
}

UCharacterMovementComponent* URJetpackComponent::GetMovementComponent () const
{
   UCharacterMovementComponent* MovementComponent = nullptr;
   if (ACharacter *Character = Cast<ACharacter> (GetOwner ())) {
      MovementComponent = Character->GetCharacterMovement ();
   }
   return MovementComponent;
}

