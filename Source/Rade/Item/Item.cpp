// Copyright 2015 Vagen Ayrapetyan

#include "Rade.h"

#include "Character/RadeCharacter.h"

#include "Item/Item.h"
#include "UnrealNetwork.h"

AItem::AItem(const class FObjectInitializer& PCIP)
	: Super(PCIP)
{

}

void AItem::BeginPlay()
{
	Super::BeginPlay();
	
}

ARadeCharacter* AItem::GetPlayer()const
{
	if (ThePlayer)return ThePlayer;
	else return NULL;
}

UWorld* AItem::GetPlayerWorld()const
{
	if (ThePlayer && ThePlayer->GetWorld())return ThePlayer->GetWorld();
	else return NULL;
}
void AItem::GetLifetimeReplicatedProps(TArray< FLifetimeProperty > & OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(AItem, ThePlayer);
}