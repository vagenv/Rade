// Copyright 2015-2017 Vagen Ayrapetyan

#include "RadeGameState.h"
#include "../Rade.h"
#include "Engine.h"
#include "Net/UnrealNetwork.h"

ARadeGameState::ARadeGameState(){
	bReplicates = true;
}

void ARadeGameState::BeginPlay(){
	Super::BeginPlay();
}

// Called On Client to Update HUD when message list updated
void ARadeGameState::OnRep_MessagesList()
{
	ChatUpdateDelegate.Broadcast();
}

// Called on server to add new message
void ARadeGameState::AddNewChatMessage(FString  TheMessage, class ARadePlayer* ThePlayer)
{
	// Add Message
	TheMessages.Add(FRadeOnineMessageData(TheMessage, ThePlayer));

	// Update HUD on server
	ChatUpdateDelegate.Broadcast();
}

// Replication of data
void ARadeGameState::GetLifetimeReplicatedProps(TArray< FLifetimeProperty > & OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(ARadeGameState, TheMessages);
}