// Copyright 2015-2016 Vagen Ayrapetyan

#include "Rade.h"
#include "RadePlayerState.h"

// Reset Values at the start of Game
void ARadePlayerState::BeginPlay()
{
	Super::BeginPlay();
	KillCount = 0;
	DeathCount = 0;
}


// Replication of data
void ARadePlayerState::GetLifetimeReplicatedProps(TArray< FLifetimeProperty > & OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(ARadePlayerState, DeathCount);
	DOREPLIFETIME(ARadePlayerState, KillCount);
	DOREPLIFETIME(ARadePlayerState, PlayerColor);
}