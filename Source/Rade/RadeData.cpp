// Copyright 2015-2021 Vagen Ayrapetyan


#include "RadeData.h"
#include "Rade.h"
#include "Weapon/Weapon.h"
#include "Custom/LevelBlock.h"
#include "Character/RadePlayer.h"


//  Online Message Data Constructor
FRadeOnineMessageData::FRadeOnineMessageData()
{
	MessageTime = FDateTime::Now();
	TheMessage  = "Undefined";
	MessageOwner = "Undefined";
	MessageColor = FColor::Magenta;
}


FRadeOnineMessageData::FRadeOnineMessageData(FString NewMessage, ARadePlayer* ThePlayer)
{
	MessageTime = FDateTime::Now();
	TheMessage  = NewMessage;
	if (ThePlayer) {
		MessageOwner = ThePlayer->CharacterName;
		MessageColor = ThePlayer->CharacterColor;
	} else {
		MessageOwner = "Undefined";
		MessageColor = FColor::Magenta;
	}
}

// Item Data Constructor
FItemData::FItemData()
{
	ItemName  = "Undefined";
	ItemIcon  = NULL;
	ItemCount = 0;
	Weight    = 0;
}

FItemData::FItemData(TSubclassOf<AItem> Item, FString newItemName, UTexture2D* newItemIcon, float newWeight , int32 newItemCount)
{
	// Set Item Data
	Archetype = Item;
	ItemName  = newItemName;
	ItemIcon  = newItemIcon;
	ItemCount = newItemCount;
	Weight    = newWeight;

	if (ItemCount <= 0)ItemCount = 1;

   AItem *newItem = Item->GetDefaultObject<AItem>();
	// if Weapon Set Main and Alt Fire
	if (Item && newItem && Cast<AWeapon>(newItem)) {
		MainFireStats = Cast<AWeapon>(newItem)->MainFire;
		AltFireStats  = Cast<AWeapon>(newItem)->AlternativeFire;
	}
}

// Create Item Data From item Object
FItemData URadeData::MakeItemDataFromItem(AItem* TheItem)
{
	FItemData TheData;
	if (TheItem) {
		TheData.ItemName  = TheItem->ItemName;
		TheData.ItemIcon  = TheItem->ItemIcon;
		TheData.ItemCount = TheItem->ItemCount;
		TheData.Weight    = TheItem->Weight;

		TheData.Archetype = TheItem->GetClass();
		if (Cast<AWeapon>(TheItem)) {
			TheData.MainFireStats = Cast<AWeapon>(TheItem)->MainFire;
			TheData.AltFireStats = Cast<AWeapon>(TheItem)->AlternativeFire;
		}

	}
	return TheData;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//						Create and Access object from Class 

UObject* URadeData::NewObjectFromBlueprint(UObject* WorldContextObject, UClass* UC)
{
	UWorld* World = GEngine->GetWorldFromContextObjectChecked(WorldContextObject);
	UObject* tempObject = NewObject<UObject>(UC);
	return tempObject;
}

UObject* URadeData::ObjectFromBlueprint(UObject* WorldContextObject, UClass* UC)
{
	if (UC && UC->GetDefaultObject()) {
		return UC->GetDefaultObject();
	}
	else return NULL;
}