// Copyright 2015-2017 Vagen Ayrapetyan

#include "Item/Item.h"
#include "Character/RadePlayer.h"
#include "Rade.h"

#include "UnrealNetwork.h"

AItem::AItem(const class FObjectInitializer& PCIP)
	: Super(PCIP)
{
}

void AItem::BeginPlay(){
	Super::BeginPlay();
}

// Get Player Pointer
ARadePlayer* AItem::GetPlayer()const
{
	if (ThePlayer)return ThePlayer;
	else return NULL;
}

// Get World Pointer
UWorld* AItem::GetPlayerWorld()const
{
	if (ThePlayer && ThePlayer->GetWorld())return ThePlayer->GetWorld();
	else return NULL;
}

// Replicate Player Reference
void AItem::GetLifetimeReplicatedProps(TArray< FLifetimeProperty > & OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(AItem, ThePlayer);
}