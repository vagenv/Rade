// Copyright 2015-2021 Vagen Ayrapetyan

#include "RadeAnimInstance.h"
#include "../Rade.h"
#include "../Character/RadePlayer.h"
#include "../Weapon/SwordWeapon.h"


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//				Base

URadeAnimInstance::URadeAnimInstance(const class FObjectInitializer& PCIP)
	: Super(PCIP)
{
}

void URadeAnimInstance::BeginPlay()
{
	if (!TheCharacter) ResetRadeCharacterRef();
	// Set default state
	PlayerCurrentAnimState = EAnimState::Idle_Run;
	AnimArchetype = EAnimArchetype::EmptyHand;
}

void URadeAnimInstance::ResetRadeCharacterRef()
{
	if (TryGetPawnOwner() && Cast<ARadeCharacter>(TryGetPawnOwner()) && TheCharacter != Cast<ARadeCharacter>(TryGetPawnOwner()))
		TheCharacter = Cast<ARadeCharacter>(TryGetPawnOwner());
}

///////////////////////////////////////////////////////////////////////////////

//										Animation Control and Events

// Set Anim State Value
void URadeAnimInstance::PlayLocalAnim(EAnimState AnimID)
{
	if (!TheCharacter)ResetRadeCharacterRef();
	PlayerCurrentAnimState = AnimID;
}

// Receive animation state value
void URadeAnimInstance::RecieveGlobalAnimID(EAnimState currentAnimCheck)
{
	if (!TheCharacter)ResetRadeCharacterRef();

	BP_AnimStarted(currentAnimCheck);
	PlayLocalAnim(currentAnimCheck);
}

// Set Animation Archetype
void URadeAnimInstance::SetAnimArchetype(EAnimArchetype newAnimArchtype)
{
	AnimArchetype = newAnimArchtype;
}


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//											State Checking

// Get Value from player class
bool URadeAnimInstance::CanFireInAir()
{
	if (TheCharacter && Cast<ARadePlayer>(TheCharacter))return Cast<ARadePlayer>(TheCharacter)->bCanFireInAir;

	return false;
}

// Called in blueprint when Jump_Start -> Jump_Idle 
void URadeAnimInstance::InAirIdleStateEntered()
{
	ARadePlayer *thePlayer = Cast<ARadePlayer>(TheCharacter);
	if (  thePlayer
		&& PlayerCurrentAnimState == EAnimState::JumpStart
		&& thePlayer->CharacterMovementComponent
		&& !thePlayer->CharacterMovementComponent->IsMovingOnGround())
		thePlayer->ServerSetAnimID(EAnimState::Jumploop);
}


// In Air State Check
bool URadeAnimInstance::IsInAir()
{
	if (	PlayerCurrentAnimState == EAnimState::JumpEnd
		|| PlayerCurrentAnimState == EAnimState::Jumploop
		|| PlayerCurrentAnimState == EAnimState::JumpStart)
		return true;
	
	return false;
}

// --- States checking
bool URadeAnimInstance::IsAnimState(EAnimState checkState)
{
	if (checkState == PlayerCurrentAnimState) return true;
	else													return false;
}
bool URadeAnimInstance::IsAnimArchetype(EAnimArchetype AnimArchtypeCheck)
{
	if (AnimArchtypeCheck == AnimArchetype) return true;
	else												 return false;
}


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//												Custom Sword Events


// Start Trace of Sword Weapon
void URadeAnimInstance::StartSwordWeaponTrace()
{
	if (!TheCharacter)ResetRadeCharacterRef();

	if (	TheCharacter
		&& TheCharacter->TheWeapon
		&& Cast<ASwordWeapon>(TheCharacter->TheWeapon))
		Cast<ASwordWeapon>(TheCharacter->TheWeapon)->StartMeleeAttackTrace();
}
// End Trace of Sword Weapon
void URadeAnimInstance::EndSwordWeaponTrace()
{
	if (	TheCharacter
		&& TheCharacter->TheWeapon
		&& Cast<ASwordWeapon>(TheCharacter->TheWeapon))
		Cast<ASwordWeapon>(TheCharacter->TheWeapon)->EndMeleeAttackTrace();
}