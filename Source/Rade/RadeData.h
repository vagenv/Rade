// Copyright 2015 Vagen Ayrapetyan

#pragma once

#include "Object.h"
#include "RadeData.generated.h"


// Camera State Enum
UENUM(BlueprintType)	
enum class ECameraState : uint8
{
	FP_Camera UMETA(DisplayName = "First Person Camera"),
	TP_Camera UMETA(DisplayName = "Third Person Camera"),
};


// Anim State Enum
UENUM(BlueprintType)	
enum class EAnimState : uint8
{
	Empty							UMETA(DisplayName = "Empty"),
	JumpStart						UMETA(DisplayName = "Jump Start"),
	JumpEnd							UMETA(DisplayName = "Jump End"),
	Jumploop 						UMETA(DisplayName = "Jump Loop"),

	Idle_Run						UMETA(DisplayName = "Idle/Run"),
	Fire							UMETA(DisplayName = "Fire"),
	Equip							UMETA(DisplayName = "Equip"),
	UnEquip							UMETA(DisplayName = "Unequip"),
	Reload							UMETA(DisplayName = "Reload"),
};


// Fire State Struct
USTRUCT()
struct FFireStats
{
	GENERATED_USTRUCT_BODY()

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
	virtual void AddAmmo(float newAmmo, int32 newClip);

	// Can Reload
	bool CanReload();

	// Can Fire
	bool CanFire();

};


// Anim Arhtype Enum
UENUM(BlueprintType)	
enum class EAnimArchetype : uint8
{
	EmptyHand					UMETA(DisplayName = "Empty Hands"),
	Pistol						UMETA(DisplayName = "Hand Gun"),
	Semi						UMETA(DisplayName = "Semi-Auto"),
	Rifle						UMETA(DisplayName = "Rifle")

};



// FIteData Struct
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
	void SetItemData(FItemData newData);
};


// Jet Pack Data Struct
USTRUCT(BlueprintType)
struct FJetPackData
{
	GENERATED_USTRUCT_BODY()

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

	FJetPackData()
	{
	}
};



// Block Data Struct 
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

	FBlockData(ALevelBlock* newLevelItem, TSubclassOf <ALevelBlock> newArchetype, FVector newGlobalPosition, FVector newConstructorPosition);
};


UCLASS()
class RADE_API URadeData: public UObject
{
	GENERATED_BODY()
};
