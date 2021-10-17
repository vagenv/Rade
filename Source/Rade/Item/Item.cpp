// Copyright 2015-2017 Vagen Ayrapetyan

#include "Item.h"
#include "../Character/RadePlayer.h"
#include "../Rade.h"

#include "Net/UnrealNetwork.h"

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
	if (ThePlayer) return ThePlayer;
	else				return NULL;
}

// Get World Pointer
UWorld* AItem::GetPlayerWorld()const
{
	if (ThePlayer && ThePlayer->GetWorld()) return ThePlayer->GetWorld();
	else												 return NULL;
}

// Item Was Used
void AItem::InventoryUse(class ARadePlayer* Player)
{
   if (ThePlayer != Player) ThePlayer = Player;

   // BP event called
   BP_ItemUsed();
};

// Replicate Player Reference
void AItem::GetLifetimeReplicatedProps(TArray< FLifetimeProperty > & OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(AItem, ThePlayer);
}