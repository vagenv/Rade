// Copyright 2015-2016 Vagen Ayrapetyan

#include "Rade.h"
#include "RadeData.h"
#include "Weapon/Weapon.h"
#include "Custom/LevelBlock.h"
#include "Character/RadePlayer.h"


//  Online Message Data Constructor
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

// Item Data Constructor
FItemData::FItemData(TSubclassOf<AItem> Item, FString newItemName, UTexture2D* newItemIcon, float newWeight , int32 newItemCount)
{

	// Set Item Data
	Archetype = Item;
	ItemName = newItemName;
	ItemIcon = newItemIcon;
	ItemCount = newItemCount;
	Weight = newWeight;

	if (ItemCount <= 0)ItemCount = 1;

	// if Weapon Set Main and Alt Fire
	if (Item && Item->GetDefaultObject<AWeapon>())
	{
		MainFireStats = Item->GetDefaultObject<AWeapon>()->MainFire;
		AltFireStats = Item->GetDefaultObject<AWeapon>()->AlternativeFire;
	}
}

// Create Item Data From item Object
FItemData URadeData::MakeItemDataFromItem(AItem* TheItem)
{
	FItemData TheData;
	if (TheItem)
	{
		TheData.ItemName = TheItem->ItemName;

		TheData.ItemIcon = TheItem->ItemIcon;

		TheData.ItemCount = TheItem->ItemCount;

		TheData.Weight = TheItem->Weight;

		TheData.Archetype = TheItem->GetClass();
		if (Cast<AWeapon>(TheItem))
		{
			TheData.MainFireStats = Cast<AWeapon>(TheItem)->MainFire;
			TheData.AltFireStats = Cast<AWeapon>(TheItem)->AlternativeFire;
		}

	}
	return TheData;
}




///////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//						Create and Acces object from Class 


UObject* URadeData::NewObjectFromBlueprint(UObject* WorldContextObject, UClass* UC)
{
	UWorld* World = GEngine->GetWorldFromContextObject(WorldContextObject);
	UObject* tempObject = NewObject<UObject>(UC);

	return tempObject;
}

UObject* URadeData::ObjectFromBlueprint(UObject* WorldContextObject, UClass* UC)
{
	if (UC && UC->GetDefaultObject())
	{
		return UC->GetDefaultObject();
	}
	else return NULL;
}