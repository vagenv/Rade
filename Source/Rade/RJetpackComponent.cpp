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
}

void URJetpackComponent::BeginPlay()
{
   Super::BeginPlay ();
   if (ACharacter *Character = Cast<ACharacter> (GetOwner ())) {
      MovementComponent = Character->GetCharacterMovement ();
   }
}

void URJetpackComponent::Use_Implementation ()
{
   if (!ensure (MovementComponent)) return;
   if (MovementComponent->IsMovingOnGround ()) return;

   URStatusMgrComponent* StatusMgr = nullptr;
   {
      TArray<URStatusMgrComponent*> StatusMgrList;
      GetOwner ()->GetComponents (StatusMgrList);
      if (StatusMgrList.Num ()) StatusMgr = StatusMgrList[0];
   }
   if (!StatusMgr) return;

   float CurrentChargePercent = StatusMgr->Stamina.Current * 100 / StatusMgr->Stamina.Max;

   if (CurrentChargePercent < MinUseablePercent) return;

   // Apply power
   MovementComponent->Velocity.Z += CurrentChargePercent * PushPower;
   StatusMgr->Stamina.Current = 0;
}
