// Copyright 2015-2023 Vagen Ayrapetyan

#include "RJetpackComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/Character.h"
#include "TimerManager.h"

URJetpackComponent::URJetpackComponent ()
{
   CurrentChargePercent = 50;
   MinUseablePercent    = 40;
   RestoreSpeed         = 0.05f;
   RestorePower         = 0.5f;
   PushPower            = 7;
}

void URJetpackComponent::BeginPlay()
{
   Super::BeginPlay ();

   ACharacter *Character = Cast<ACharacter> (GetOwner ());
   if (Character) {
      MovementComponent = Character->GetCharacterMovement();

      FTimerHandle FillUpHandle;
      Character->GetWorldTimerManager().SetTimer (FillUpHandle, this, &URJetpackComponent::FillUp, RestoreSpeed, true);
   }
}

void URJetpackComponent::Use_Implementation ()
{
   if (!MovementComponent) return;
   if (MovementComponent->IsMovingOnGround ()) return;

   if (CurrentChargePercent < MinUseablePercent) return;

   // Apply power
   MovementComponent->Velocity.Z += CurrentChargePercent * PushPower;
   CurrentChargePercent = 0;
}

void URJetpackComponent::FillUp ()
{
   if (!MovementComponent) return;

   if (MovementComponent->IsMovingOnGround ()) {
      if (CurrentChargePercent < 100) {
         CurrentChargePercent += RestorePower;
         if (CurrentChargePercent > 100) {
            CurrentChargePercent = 100;
         }
      }
   }
}