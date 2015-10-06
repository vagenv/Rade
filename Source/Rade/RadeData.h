// Copyright 2015 Vagen Ayrapetyan

#pragma once

#include "Object.h"
#include "RadeData.generated.h"

UENUM(BlueprintType)
namespace EItemType
{
	//256 entries max
	enum Type
	{
		Base		UMETA(DisplayName = "Base"),
		Useable		UMETA(DisplayName = "Useable / Event Driven"),
		Quest		UMETA(DisplayName = "Quest Item"),
		Weapon		UMETA(DisplayName = "Weapon"),
		Armor		UMETA(DisplayName = "Armor"),
	};
}

UENUM(BlueprintType)		//"BlueprintType" is essential to include
enum class ECameraState : uint8
{
	FP_Camera UMETA(DisplayName = "First Person Camera"),
	TP_Camera UMETA(DisplayName = "Third Person Camera"),
};




UENUM(BlueprintType)		//"BlueprintType" is essential to include
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

USTRUCT()
struct FFireStats
{
	GENERATED_USTRUCT_BODY()


	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "")
		int32 ClipNumber = 0;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "")
		int32 MaxClipNumber = 1;


	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "")
		float ClipSize = 10;


	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "")
		float CurrentAmmo = 8;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "")
		float FireDamage = 1;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "")
		float FireSpeed = 1;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "")
		float FireCost = 1;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "")
		float FireDistance = 2500;


	virtual void AddAmmo(float newAmmo, int32 newClip);

	bool CanReload();
	bool CanFire();

};



UENUM(BlueprintType)		//"BlueprintType" is essential to include
enum class EAnimArchetype : uint8
{
	EmptyHand					UMETA(DisplayName = "Empty Hand"),
	Pistol						UMETA(DisplayName = "Hand Gun"),
	Semi						UMETA(DisplayName = "Semi-Auto"),
	Rifle						UMETA(DisplayName = "Rifle")

};




USTRUCT(BlueprintType)
struct FItemData
{
	GENERATED_USTRUCT_BODY()
public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Item")
		FString ItemName;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Item")
		UTexture2D* ItemIcon;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Item")
		int32 ItemCount;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Item")
		float Weight;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Item")
		TSubclassOf<class AItem> Archetype;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Item")
		FFireStats MainFireStats;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Item")
		FFireStats AltFireStats;
	//UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Item")
	//FAdditionalItemData AdditionalData;

	FItemData(){};
	FItemData(TSubclassOf<class AItem> Item, FString newItemName = FString(""), UTexture2D* newItemIcon = NULL, float newWeight = 0.1, int32 newItemCount = 1);

	void SetItemData(FItemData newData);
	/*
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
	*/
};


USTRUCT(BlueprintType)
struct FJetPackProp
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "")
		float CurrentChargePercent=50;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "")
		float MinUseablePercent=40;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "")
		float RestoreSpeed=0.1f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "")
		float RestorePower=0.5f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "")
		float PushPower=1;

	FJetPackProp()
	{
	}
};




USTRUCT(BlueprintType)
struct FBlockData
{
	GENERATED_USTRUCT_BODY()

public:

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Block")
		class ALevelBlock* LevelItem;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Block")
		TSubclassOf <class ALevelBlock> Archetype;

	//FName ArchetypePath;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Block")
		FVector GlobalPosition;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Block")
		FVector ConstructorPosition;

	FBlockData(){};

	FBlockData(ALevelBlock* newLevelItem, TSubclassOf <ALevelBlock> newArchetype, FVector newGlobalPosition, FVector newConstructorPosition);
		/*
	{
		LevelItem = newLevelItem;
		Archetype = newArchetype;
		GlobalPosition = newGlobalPosition;
		ConstructorPosition = newConstructorPosition;
	}
	*/
};


UCLASS()
class RADE_API URadeData: public UObject
{
	GENERATED_BODY()
};
