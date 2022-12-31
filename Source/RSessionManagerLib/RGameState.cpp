// Copyright 2015-2023 Vagen Ayrapetyan

#include "RGameState.h"
#include "Engine.h"
#include "Net/UnrealNetwork.h"

//  Online Message Data Constructor
FROnineMessageData::FROnineMessageData ()
   : Time (FDateTime::Now ())
{
}

FROnineMessageData::FROnineMessageData (FString Message_, FString Owner_, FColor Color_)
   : Time (FDateTime::Now ()),
     Message (Message_),
     Owner (Owner_),
     Color (Color_)
{
}

ARGameState::ARGameState ()
{
   bReplicates = true;
}

void ARGameState::BeginPlay ()
{
   Super::BeginPlay ();
}

// Called On Client to Update HUD when message list updated
void ARGameState::OnRep_MessagesList ()
{
   ChatUpdateDelegate.Broadcast ();
}

// Called on server to add new message
void ARGameState::AddChatMessage (FString Message_, FString Owner_, FColor Color_)
{
   // Add Message
   Messages.Add (FROnineMessageData (Message_, Owner_, Color_));

   // Update HUD on server
   ChatUpdateDelegate.Broadcast ();
}

// Replication of data
void ARGameState::GetLifetimeReplicatedProps (TArray< FLifetimeProperty > & OutLifetimeProps) const
{
   Super::GetLifetimeReplicatedProps (OutLifetimeProps);
   DOREPLIFETIME(ARGameState, Messages);
}

