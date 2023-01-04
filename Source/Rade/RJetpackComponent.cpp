// Copyright 2015-2023 Vagen Ayrapetyan

#include "RJetpackComponent.h"
#include "RUtilLib/Rlog.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/Character.h"
#include "Math/UnrealMathUtility.h"

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
   RegisterComponent ();
}

void URJetpackComponent::TickComponent (float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction *ThisTickFunction)
{
   Super::TickComponent (DeltaTime, TickType, ThisTickFunction);
   FillUp (DeltaTime);
}

void URJetpackComponent::Use_Implementation ()
{
   if (!ensure (MovementComponent)) return;
   if (MovementComponent->IsMovingOnGround ()) return;

   if (CurrentChargePercent < MinUseablePercent) return;

   // Apply power
   MovementComponent->Velocity.Z += CurrentChargePercent * PushPower;
   CurrentChargePercent = 0;
}

void URJetpackComponent::FillUp (float DeltaTime)
{
   if (!ensure (MovementComponent)) return;

   if (MovementComponent->IsMovingOnGround ()) {
      if (CurrentChargePercent < 100) {
         CurrentChargePercent += (RestorePower * DeltaTime);
         CurrentChargePercent = FMath::Clamp (CurrentChargePercent, 0 , 100);
      }
   }
}

