// Copyright 2015-2023 Vagen Ayrapetyan

#include "RPlayerState.h"
#include "Net/UnrealNetwork.h"

void ARPlayerState::BeginPlay ()
{
   Super::BeginPlay ();

   // Reset Values at the start of Game
   nDeaths = 0;
   nKills  = 0;
}

// Replication of data
void ARPlayerState::GetLifetimeReplicatedProps (TArray< FLifetimeProperty > & OutLifetimeProps) const
{
   Super::GetLifetimeReplicatedProps (OutLifetimeProps);
   DOREPLIFETIME(ARPlayerState, nDeaths);
   DOREPLIFETIME(ARPlayerState, nKills);
   DOREPLIFETIME(ARPlayerState, PlayerColor);
}

