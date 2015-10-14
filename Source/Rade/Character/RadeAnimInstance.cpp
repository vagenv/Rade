// Copyright 2015 Vagen Ayrapetyan

#include "Rade.h"
#include "Character/RadeAnimInstance.h"
#include "Character/RadeCharacter.h"


URadeAnimInstance::URadeAnimInstance(const class FObjectInitializer& PCIP)
	: Super(PCIP)
{
}


void URadeAnimInstance::BeginPlay()
{
	// Set default state
	PlayerCurrentAnimState = EAnimState::Idle_Run;
	AnimArchetype = EAnimArchetype::EmptyHand;
}

// Set Anim State Value
void URadeAnimInstance::PlayLocalAnim(EAnimState AnimID)
{
	PlayerCurrentAnimState = AnimID;
}

// Recieve anim state value
void URadeAnimInstance::RecieveGlobalAnimID(EAnimState currentAnimCheck)
{
	AnimStartedBPEvent(currentAnimCheck);
	PlayLocalAnim(currentAnimCheck);
}

// Get Value from player class
bool URadeAnimInstance::CanFireInAir()
{
	if (ThePlayer)return ThePlayer->bCanFireInAir;

	return false;
}


// Called in blueprint when Jump_Start -> Jump_Idle 
void URadeAnimInstance::InAirIdleStateEntered()
{
	if (ThePlayer && PlayerCurrentAnimState == EAnimState::JumpStart && 
		ThePlayer->PlayerMovementComponent && !ThePlayer->PlayerMovementComponent->IsMovingOnGround())
		ThePlayer->ServerSetAnimID(EAnimState::Jumploop);
}

// In Air State Check
bool URadeAnimInstance::IsInAir()
{
	if (PlayerCurrentAnimState == EAnimState::JumpEnd || PlayerCurrentAnimState == EAnimState::Jumploop || PlayerCurrentAnimState == EAnimState::JumpStart)return true;
	else return false;
}

// States checking

bool URadeAnimInstance::IsAnimState(EAnimState checkState)
{
	if (checkState == PlayerCurrentAnimState)return true;
	else return false;
}
bool URadeAnimInstance::IsAnimArchetype(EAnimArchetype AnimArchtypeCheck)
{
	if (AnimArchtypeCheck == AnimArchetype)return true;
	else return false;
}
