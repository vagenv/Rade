// Copyright 2015 Vagen Ayrapetyan

#pragma once

#include "GameFramework/Actor.h"
#include "RadeData.h"
#include "Item.generated.h"

// Base for Any Item or Weapon
UCLASS()
class RADE_API AItem : public AActor
{
	GENERATED_BODY()
public:	

	AItem(const class FObjectInitializer& PCIP);

	virtual void BeginPlay() override;

	//////////////////////////////////////////////////////////////////////////////////////////////////
	
	//				Main Components and Refernces

	// Pickup Mesh
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Pickup")
		UStaticMesh* PickupMesh;

	// Pickup Skeletal mesg
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Pickup")
		USkeletalMesh* PickupSkelMesh;

	// Player Pointer
	UPROPERTY(Replicated)
		class ARadePlayer* ThePlayer;

	//////////////////////////////////////////////////////////////////////////////////////////////////

	//				Main Properties

	// Item Name
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Item")
		FString ItemName = FString("Name of Item");

	// Item Icon
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Item")
		UTexture2D* ItemIcon;

	// Item Count
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Item")
		int32 ItemCount=1;

	// Item Weight
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Item")
		float Weight =1;

	// Item Pickup Override Archtype
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Item")
		TSubclassOf<AItemPickup> ItemPickupArchetype;


	//////////////////////////////////////////////////////////////////////////////////////////////////

	//				Item Events


	// BP Event- Item Was Used
	UFUNCTION(BlueprintImplementableEvent, Category = "Rade")
		void BP_ItemUsed();

	// BP Event- Item Was Dropped
	UFUNCTION(BlueprintImplementableEvent, Category = "Rade")
		void BP_ItemDroped(class AItemPickup* thePickup);

	// BP Event- Item Was Updated
	UFUNCTION(BlueprintImplementableEvent, Category = "Rade")
		void BP_ItemUpdated();

	// Get Player Ref
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Rade")
		class ARadePlayer* GetPlayer()const;

	// Get World Ref
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Rade")
		class UWorld* GetPlayerWorld()const;


	// Item Was Used
	virtual void InventoryUse(class ARadePlayer* Player)
	{
		// Set Player Ref if player ref in empty or wrong
		if (ThePlayer == Player || ThePlayer != Player)
		{
			ThePlayer = Player;
		}

		// BP event called
		BP_ItemUsed();
	};
};
