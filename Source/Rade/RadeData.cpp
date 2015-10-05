// Copyright 2015 Vagen Ayrapetyan

#include "Rade.h"
#include "RadeData.h"
#include "Weapon/Weapon.h"
#include "Custom/LevelBlock.h"

FItemData::FItemData(TSubclassOf<AItem> Item, FString newItemName, UTexture2D* newItemIcon, float newWeight , int32 newItemCount)
{
	//	print("new ItemData");

	Archetype = Item;
	ItemName = newItemName;
	ItemIcon = newItemIcon;
	ItemCount = newItemCount;
	Weight = newWeight;

	if (Item && Item->GetDefaultObject<AWeapon>())
	{
		WeaponStats = Item->GetDefaultObject<AWeapon>()->MainFire;
	}
}

FBlockData::FBlockData(ALevelBlock* newLevelItem, TSubclassOf <ALevelBlock> newArchetype, FVector newGlobalPosition, FVector newConstructorPosition)
{
	LevelItem = newLevelItem;
	Archetype = newArchetype;
	GlobalPosition = newGlobalPosition;
	ConstructorPosition = newConstructorPosition;
}



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

bool FFireStats::CanReload()
{
	if (ClipNumber > 0 && CurrentAmmo < ClipSize)
	{
		return true;
	}

	return false;
}

bool FFireStats::CanFire()
{
	if (FireCost > 0 && FireCost > CurrentAmmo)return false;

	return true;
}


