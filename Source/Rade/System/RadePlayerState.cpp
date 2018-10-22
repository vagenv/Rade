// Copyright 2015-2017 Vagen Ayrapetyan

#include "RadePlayerState.h"
#include "Rade.h"
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