// Copyright 2015 Vagen Ayrapetyan

#include "Rade.h"
#include "RadeData.h"
#include "Weapon/Weapon.h"
#include "Custom/LevelBlock.h"
#include "Character/RadePlayer.h"




FRadeOnineMessageData::FRadeOnineMessageData(FString NewMessage, ARadePlayer* ThePlayer)
{

	MessageTime = FDateTime::Now();
	TheMessage = NewMessage;
	if (ThePlayer)
	{
		MessageOwner = ThePlayer->CharacterName;
		MessageColor = ThePlayer->CharacterColor;
	}


}


FAvaiableSessionsData::FAvaiableSessionsData(FOnlineSessionSearchResult newSessionData)
{
	SessionData = newSessionData;

	OwnerName = newSessionData.Session.OwningUserName;
	Ping = newSessionData.PingInMs;
	NumberOfConnections = newSessionData.Session.SessionSettings.NumPublicConnections;
	NumberOfAvaiableConnections = NumberOfConnections - newSessionData.Session.NumOpenPublicConnections;
}







FItemData::FItemData(TSubclassOf<AItem> Item, FString newItemName, UTexture2D* newItemIcon, float newWeight , int32 newItemCount)
{

	// Set Item Data
	Archetype = Item;
	ItemName = newItemName;
	ItemIcon = newItemIcon;
	ItemCount = newItemCount;
	Weight = newWeight;

	// if Weapon Set Main and Alt Fire
	if (Item && Item->GetDefaultObject<AWeapon>())
	{
		MainFireStats = Item->GetDefaultObject<AWeapon>()->MainFire;
		AltFireStats = Item->GetDefaultObject<AWeapon>()->AlternativeFire;
	}
}

// Set Item Data from other data
void FItemData::SetItemData(FItemData newData)
{
	ItemName=newData.ItemName;

	ItemIcon=newData.ItemIcon;

	ItemCount=newData.ItemCount;

	Weight=newData.Weight;

	Archetype=newData.Archetype;

	MainFireStats=newData.MainFireStats;
	AltFireStats=newData.AltFireStats;
}

FBlockData::FBlockData(ALevelBlock* newLevelItem, TSubclassOf <ALevelBlock> newArchetype, FVector newGlobalPosition, FVector newConstructorPosition)
{
	LevelItem = newLevelItem;
	Archetype = newArchetype;
	GlobalPosition = newGlobalPosition;
	ConstructorPosition = newConstructorPosition;
}


// Add Ammo To Fire Stats
void FFireStats::AddAmmo(float newAmmo, int32 newClip)
{
	if (newAmmo>0)
	{
		if (CurrentAmmo + newAmmo <= ClipSize) CurrentAmmo += newAmmo;
		else newClip++;
	}
	if (newClip>0)
	{
		if (newClip + ClipNumber <= MaxClipNumber) ClipNumber += newClip;
		else ClipNumber = MaxClipNumber;
	}
}

// Can Reload 
bool FFireStats::CanReload()
{
	if (ClipNumber > 0 && CurrentAmmo < ClipSize)
	{
		return true;
	}
	return false;
}

// Can Fire
bool FFireStats::CanFire()
{
	if (FireCost > 0 && FireCost > CurrentAmmo)return false;
	return true;
}


