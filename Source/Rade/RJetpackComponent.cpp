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

   URStatusMgrComponent* StatusMgr = URStatusMgrComponent::Get (GetOwner ());
   if (!StatusMgr) return false;

   FRStatusValue Mana = StatusMgr->GetMana ();
   return Mana.Current > UseCost;
}

void URJetpackComponent::Use_Implementation ()
{
   if (!CanUse ()) return;

   UCharacterMovementComponent* MovementComponent = GetMovementComponent ();
   if (!MovementComponent) return;
   URStatusMgrComponent* StatusMgr = URStatusMgrComponent::Get (GetOwner ());
   if (!StatusMgr) return;

   const FRichCurve* AgiToJumpPowerData = AgiToJumpPower.GetRichCurveConst ();
   if (!ensure (AgiToJumpPowerData)) return;

   FRStatusValue Mana = StatusMgr->GetMana ();
   // Apply power
   float JumpVelocity = AgiToJumpPowerData->Eval (StatusMgr->GetCoreStats_Total ().AGI);

   //R_LOG_PRINTF ("Jump power : %f", JumpVelocity);
   // TODO: Get Move Vector, Normalize.
   MovementComponent->Velocity.Z += JumpVelocity;

   StatusMgr->UseMana (UseCost);
}


UCharacterMovementComponent* URJetpackComponent::GetMovementComponent () const
{
   UCharacterMovementComponent* MovementComponent = nullptr;
   if (ACharacter *Character = Cast<ACharacter> (GetOwner ())) {
      MovementComponent = Character->GetCharacterMovement ();
   }
   return MovementComponent;
}

