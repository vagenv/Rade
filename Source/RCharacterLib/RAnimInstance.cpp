// Copyright 2015-2023 Vagen Ayrapetyan

#include "RAnimInstance.h"
#include "RCharacter.h"
//#include "../Rade.h"
//#include "../Character/RadePlayer.h"
//#include "../Weapon/SwordWeapon.h"
//#include "Net/UnrealNetwork.h"


//=============================================================================
//            Base
//=============================================================================

URAnimInstance::URAnimInstance()
{

}

void URAnimInstance::BeginPlay()
{

   //if (!TheCharacter) ResetRadeCharacterRef();
   // Set default state
   PlayerCurrentAnimState = ERAnimState::Idle_Run;
   AnimArchetype = ERAnimArchetype::EmptyHand;

}


void URAnimInstance::ResetRadeCharacterRef()
{
   // if (TryGetPawnOwner() && Cast<ARCharacter>(TryGetPawnOwner()) && TheCharacter != Cast<ARCharacter>(TryGetPawnOwner()))
   //    TheCharacter = Cast<ARCharacter>(TryGetPawnOwner());
}

//=============================================================================
//                     Animation Control and Events
//=============================================================================

// Set Anim State Value
void URAnimInstance::PlayLocalAnim(ERAnimState AnimID)
{
   // if (!TheCharacter)ResetRadeCharacterRef();
   PlayerCurrentAnimState = AnimID;
}

// Receive animation state value
void URAnimInstance::RecieveGlobalAnimID(ERAnimState currentAnimCheck)
{
   // if (!TheCharacter)ResetRadeCharacterRef();

   BP_AnimStarted(currentAnimCheck);
   PlayLocalAnim(currentAnimCheck);
}

// Set Animation Archetype
void URAnimInstance::SetAnimArchetype(ERAnimArchetype newAnimArchtype)
{
   AnimArchetype = newAnimArchtype;
}


//=============================================================================
//                      State Checking
//=============================================================================

// Get Value from player class
bool URAnimInstance::CanFireInAir()
{
   //if (TheCharacter && Cast<ARadePlayer>(TheCharacter))return Cast<ARadePlayer>(TheCharacter)->bCanFireInAir;

   return false;
}

// Called in blueprint when Jump_Start -> Jump_Idle
void URAnimInstance::InAirIdleStateEntered()
{
   /*
   ARadePlayer *thePlayer = Cast<ARadePlayer>(TheCharacter);
   if (  thePlayer
      && PlayerCurrentAnimState == ERAnimState::JumpStart
      //&& thePlayer->CharacterMovementComponent
      //&& !thePlayer->CharacterMovementComponent->IsMovingOnGround()
      )
      thePlayer->ServerSetAnimID(ERAnimState::Jumploop);
   */
}


// In Air State Check
bool URAnimInstance::IsInAir()
{
   if (  PlayerCurrentAnimState == ERAnimState::JumpEnd
      || PlayerCurrentAnimState == ERAnimState::Jumploop
      || PlayerCurrentAnimState == ERAnimState::JumpStart)
      return true;

   return false;
}

// --- States checking
bool URAnimInstance::IsAnimState(ERAnimState checkState)
{
   if (checkState == PlayerCurrentAnimState) return true;
   else                                       return false;
}
bool URAnimInstance::IsAnimArchetype(ERAnimArchetype AnimArchtypeCheck)
{
   if (AnimArchtypeCheck == AnimArchetype) return true;
   else                                     return false;
}


//=============================================================================
//                            Custom Sword Events
//=============================================================================

// Start Trace of Sword Weapon
void URAnimInstance::StartSwordWeaponTrace()
{

   // if (!TheCharacter) ResetRadeCharacterRef();

   /*
   if (   TheCharacter
      && TheCharacter->TheWeapon
      && Cast<ASwordWeapon>(TheCharacter->TheWeapon))
      Cast<ASwordWeapon>(TheCharacter->TheWeapon)->StartMeleeAttackTrace();
   */
}

// End Trace of Sword Weapon
void URAnimInstance::EndSwordWeaponTrace()
{
   /*
   if (  TheCharacter
      && TheCharacter->TheWeapon
      && Cast<ASwordWeapon>(TheCharacter->TheWeapon))
      Cast<ASwordWeapon>(TheCharacter->TheWeapon)->EndMeleeAttackTrace();
      */
}


// Replication
void URAnimInstance::GetLifetimeReplicatedProps(TArray< FLifetimeProperty > & OutLifetimeProps) const
{
   Super::GetLifetimeReplicatedProps(OutLifetimeProps);
/*
   DOREPLIFETIME(URAnimInstance, AnimArchetype);
   DOREPLIFETIME(URAnimInstance, PlayerCurrentAnimState);
   */
}

