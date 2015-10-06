// Copyright 2015 Vagen Ayrapetyan

#include "Rade.h"
#include "Character/RadeAnimInstance.h"
#include "Character/RadeCharacter.h"


URadeAnimInstance::URadeAnimInstance(const class FObjectInitializer& PCIP)
	: Super(PCIP)
{

	bInAir = false;
}


void URadeAnimInstance::BeginPlay()
{

	PlayerCurrentAnimState = EAnimState::Idle_Run;

	EAnimArchetype Anim = EAnimArchetype::Pistol;
	AnimArchetype = Anim;

}

void URadeAnimInstance::PlayLocalAnim(EAnimState AnimID)
{
	PlayerCurrentAnimState = AnimID;

	/*
	if (bOriginalAnimInstance && thePlayer)
	{
		//	print("Send anim id");
		thePlayer->ServerSetAnimID((uint8)PlayerCurrentAnimState);
	}
	*/
}

void URadeAnimInstance::RecieveGlobalAnimID(EAnimState currentAnimCheck)
{
	//if (bOriginalAnimInstance)return;

	AnimStartedBPEvent(currentAnimCheck);
	PlayLocalAnim(currentAnimCheck);
}

// Animation Ended from Bluprint
void URadeAnimInstance::AnimEnded(EAnimState currentAnimState)
{

}
bool URadeAnimInstance::CanFireInAir()
{
	if (ThePlayer)return ThePlayer->bCanFireInAir;

	return false;
}


void URadeAnimInstance::InAirIdleStateEntered()
{
	if (PlayerCurrentAnimState == EAnimState::JumpStart && ThePlayer && ThePlayer->PlayerMovementComponent && !ThePlayer->PlayerMovementComponent->IsMovingOnGround())
		PlayerCurrentAnimState = EAnimState::Jumploop;
}

bool URadeAnimInstance::IsInAir()
{
	if (PlayerCurrentAnimState == EAnimState::JumpEnd || PlayerCurrentAnimState == EAnimState::Jumploop || PlayerCurrentAnimState == EAnimState::JumpStart)return true;
	else return false;
}

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


EAnimState URadeAnimInstance::GetCurrentAnimID()
{
	return PlayerCurrentAnimState;
}