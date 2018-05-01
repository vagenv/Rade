// Copyright 2015-2017 Vagen Ayrapetyan

#pragma once

#include "Object.h"
#include "Online.h"
#include "RadeData.generated.h"


// Camera State Type
UENUM(BlueprintType)
enum class ECameraState : uint8
{
	FP_Camera UMETA(DisplayName = "First Person Camera"),
	TP_Camera UMETA(DisplayName = "Third Person Camera"),
};


////////////////////////////////////////////////////////////////////////////////////////

//							Custom Oline Data Type


//  Online Message Data
USTRUCT(BlueprintType)
struct FRadeOnineMessageData
{
	GENERATED_USTRUCT_BODY()
public:
	// The Actual Message
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "")
		FString TheMessage;

	// Player Name of the Message Owner
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "")
		FString MessageOwner;

	// Date of Message
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "")
		FDateTime MessageTime;

	// Message Color
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "")
		FLinearColor MessageColor;


	FRadeOnineMessageData(){};
	FRadeOnineMessageData(FString NewMessage, class ARadePlayer* ThePlayer);
};


// Online Session Data Struct
USTRUCT(BlueprintType)
struct FAvaiableSessionsData
{
	GENERATED_USTRUCT_BODY()
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "")
		FString OwnerName;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "")
		int32 Ping;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "")
		int32 NumberOfConnections;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "")
		int32 NumberOfAvaiableConnections;

	FOnlineSessionSearchResult SessionData;

	FAvaiableSessionsData(){};
	FAvaiableSessionsData(FOnlineSessionSearchResult newSessionData)
	{
		SessionData = newSessionData;

		OwnerName = newSessionData.Session.OwningUserName;
		Ping = newSessionData.PingInMs;
		NumberOfConnections = newSessionData.Session.SessionSettings.NumPublicConnections;
		NumberOfAvaiableConnections = NumberOfConnections - newSessionData.Session.NumOpenPublicConnections;
	}

};

////////////////////////////////////////////////////////////////////////////////////////

//							Custom Anim Type

// Weapon Anim Archetype Enum
UENUM(BlueprintType)	
enum class EAnimArchetype : uint8
{
	EmptyHand							UMETA(DisplayName = "Empty Hands"),
	Melee_Weapon						UMETA(DisplayName = "Melee Weapon"),
	Handgun_Weapon						UMETA(DisplayName = "Hand Gun"),
	Hip_Weapon							UMETA(DisplayName = "Hip Fired Weapon"),
	Shoulder_Weapon						UMETA(DisplayName = "Shoulder Fired Weapon")

};

// Anim State Type
UENUM(BlueprintType)
enum class EAnimState : uint8
{
	Empty							UMETA(DisplayName = "Empty"),

	JumpStart						UMETA(DisplayName = "Jump Start"),
	JumpEnd							UMETA(DisplayName = "Jump End"),
	Jumploop 						UMETA(DisplayName = "Jump Loop"),

	Idle_Run						UMETA(DisplayName = "Idle/Run"),
	Melee_Attack					UMETA(DisplayName = "Melee Attack"),

	Fire							UMETA(DisplayName = "Fire"),
	Equip							UMETA(DisplayName = "Equip"),
	UnEquip							UMETA(DisplayName = "Unequip"),
	Reload							UMETA(DisplayName = "Reload"),
};


////////////////////////////////////////////////////////////////////////////////////////

//							Item and Weapon Data Struct 


// Fire Data Struct
USTRUCT(BlueprintType)
struct FFireStats
{
	GENERATED_USTRUCT_BODY()
public:
	// Current Number of clips/Magazines
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "")
		int32 ClipNumber = 0;

	// Max Number of clips/Magazines
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "")
		int32 MaxClipNumber = 1;

	// Numer of bullets in clip
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "")
		float ClipSize = 10;

	// Current number of bullets |  In current Magazine
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "")
		float CurrentAmmo = 8;

	// Fire Damage
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "")
		float FireDamage = 1;

	// Fire Speed
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "")
		float FireSpeed = 1;

	// Cost of Fire
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "")
		float FireCost = 1;

	// Fire Distance
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "")
		float FireDistance = 2500;

	// Add amoo 
	virtual void AddAmmo(float newAmmo, int32 newClip)
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
	bool CanReload(){
		if (ClipNumber > 0 && CurrentAmmo < ClipSize)
			return true;
		return false;
	}

	// Can Fire
	bool CanFire(){
		if (FireCost > 0 && FireCost > CurrentAmmo) return false;
		return true;
	}

	FFireStats(){};
	virtual ~FFireStats(){};

};


// Item Data Struct 
USTRUCT(BlueprintType)
struct FItemData
{
	GENERATED_USTRUCT_BODY()
public:

	// Item Name
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Item")
		FString ItemName;

	//Item Icon
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Item")
		UTexture2D* ItemIcon;

	// Item Count
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Item")
		int32 ItemCount;

	// Ite Weight
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Item")
		float Weight;

	// Item Archtype/Blueprint
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Item")
		TSubclassOf<class AItem> Archetype;

	// If Weapon , main fire data
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Item")
		FFireStats MainFireStats;

	// If Weapon, alt fire data
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Item")
		FFireStats AltFireStats;

	FItemData(){};
	FItemData(TSubclassOf<class AItem> Item, FString newItemName = FString(""), UTexture2D* newItemIcon = NULL, float newWeight = 0.1, int32 newItemCount = 1);

	// Set Item Data
	void SetItemData(FItemData newData)
	{
		ItemName = newData.ItemName;

		ItemIcon = newData.ItemIcon;

		ItemCount = newData.ItemCount;

		Weight = newData.Weight;

		Archetype = newData.Archetype;

		MainFireStats = newData.MainFireStats;
		AltFireStats = newData.AltFireStats;
	}
};

////////////////////////////////////////////////////////////////////////////////////////

//						 Jet Pack Data Struct

USTRUCT(BlueprintType)
struct FJetPackData
{
	GENERATED_USTRUCT_BODY()
public:
	// Curret Charage Percent
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "")
		float CurrentChargePercent=50;

	// Minimal Useable Charge
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "")
		float MinUseablePercent=40;

	// restore Speed
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "")
		float RestoreSpeed=0.1f;

	// Restore Value
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "")
		float RestorePower=0.5f;

	// Push Mutiplier
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "")
		float PushPower=1;

	FJetPackData(){}
};

////////////////////////////////////////////////////////////////////////////////////////

//							Block Data Struct 

USTRUCT(BlueprintType)
struct FBlockData
{
	GENERATED_USTRUCT_BODY()

public:

	// Levet block pointer 
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Block")
		class ALevelBlock* LevelItem;

	// Level block archtype/blueprint
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Block")
		TSubclassOf <class ALevelBlock> Archetype;

	// Location in the world
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Block")
		FVector GlobalPosition;

	// Location in the constructor
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Block")
		FVector ConstructorPosition;

	FBlockData(){};

	FBlockData(ALevelBlock* newLevelItem, TSubclassOf <ALevelBlock> newArchetype, FVector newGlobalPosition, FVector newConstructorPosition)
	{
		LevelItem = newLevelItem;
		Archetype = newArchetype;
		GlobalPosition = newGlobalPosition;
		ConstructorPosition = newConstructorPosition;
	}
};

// Container of Custom Types
UCLASS()
class RADE_API URadeData: public UObject
{
	GENERATED_BODY()

public:


	// Create Item Data From Item Object
	UFUNCTION(BlueprintPure, meta = (HidePin = "WorldContextObject", DefaultToSelf = "WorldContextObject", DisplayName = "ItemData From Item", CompactNodeTitle = "ItemData", Keywords = "itemdata from item"), Category = "Rade")
		static FItemData MakeItemDataFromItem(AItem* TheItem);

	// Create New Object from Class
	UFUNCTION(BlueprintPure, meta = (HidePin = "WorldContextObject", DefaultToSelf = "WorldContextObject", DisplayName = "Create Object From Blueprint", CompactNodeTitle = "Create", Keywords = "new create blueprint"), Category = "Rade")
	static UObject* NewObjectFromBlueprint(UObject* WorldContextObject, UClass* UC);

	// Access Object from Class
	UFUNCTION(BlueprintPure, meta = (HidePin = "WorldContextObject", DefaultToSelf = "WorldContextObject", DisplayName = "Acces Object From Blueprint", CompactNodeTitle = "Access", Keywords = "access create blueprint"), Category = "Rade")
	static UObject* ObjectFromBlueprint(UObject* WorldContextObject, UClass* UC);

};
