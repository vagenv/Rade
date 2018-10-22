// Copyright 2015-2017 Vagen Ayrapetyan
#pragma once

#include "Components/ActorComponent.h"
#include "RadeData.h"
#include "Inventory.generated.h"

// Inventory Component. Holds all Items Character Picked Up
UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class RADE_API UInventory : public UActorComponent
{
	GENERATED_BODY()
public:	

	// Base events
	UInventory(const class FObjectInitializer& PCIP);
	virtual void BeginPlay() override;


	////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	//								Imporant References and Properties

	// The Character (Owner)
	UPROPERTY(Replicated, VisibleAnywhere, BlueprintReadOnly, Category = "Rade")
		class ARadeCharacter* TheCharacter;

	// Game Mode Pointer , To Save and Load Information
		UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Rade")
		class ARadeGameMode* TheGM;

	// Item Data List
	UPROPERTY(ReplicatedUsing = OnRep_ItemListUpdated, VisibleAnywhere, BlueprintReadOnly, Category = "Rade")
		TArray<FItemData> Items;


	// Total Weight of Inventory
	UPROPERTY(Replicated, VisibleAnywhere, BlueprintReadOnly, Category = "Rade")
		float TotalWeight;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Rade")
		bool bDropItemsOnDeath = false;


	////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	//								Internal Functions



	// Add new Item Data 
	FItemData* AddItem(TSubclassOf<AItem> NewItem);

	// Item was picked up
	void ItemPickedUp(class AItemPickup* ThePickup);

	////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	//								Callable Events

	// Add new Item Data, Called from BP
	UFUNCTION(BlueprintCallable, Category = "Rade")
		FItemData AddNewItem(TSubclassOf<AItem> NewItem);
	

	// Check/Update Item Data List
	UFUNCTION(BlueprintCallable, Category = "Rade")
		void UpdateInfo();


	// Use Item with Index
	UFUNCTION(BlueprintCallable, Category = "Rade")
		void ActionIndex(int32 ItemID);

	// Use item With pointer
	UFUNCTION(BlueprintCallable, Category = "Rade")
		void Action(AItem* ItemRef);

	// Throw Out item With Index
	UFUNCTION(BlueprintCallable, Category = "Rade")
		void ThrowOutIndex(int32 ItemID);

	// Throw Out item with pointer
	UFUNCTION(BlueprintCallable, Category = "Rade")
		void ThrowOut(AItem* ItemRef);


	// Throw Out item with pointer
	UFUNCTION(BlueprintCallable, Category = "Rade")
		void ThrowOutAllItems();


	// Remove Item with Idex From Item List 
	UFUNCTION(BlueprintCallable, Category = "Rade")
		void RemoveItemIndex(int32 ItemID);

	// Rmove item with item pointer from Item List
	UFUNCTION(BlueprintCallable, Category = "Rade")
		void RemoveItem(AItem* DeleteItem);



	// Remove Item with Idex From Item List 
	UFUNCTION(BlueprintCallable, Category = "Rade")
		void RemoveItemIndex_Count(int32 ItemID, int32 ItemRemoveCount);

	// Rmove item with item pointer from Item List
	UFUNCTION(BlueprintCallable, Category = "Rade")
		void RemoveItem_Count(AItem* DeleteItem, int32 ItemRemoveCount);





	// Item List Updated, Called on Clients
	UFUNCTION()
		void OnRep_ItemListUpdated();


	////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	//								Save/ Load 

	// Load Inventory
	UFUNCTION(BlueprintCallable, Category = "Rade")
		void LoadInventory();

	// Save inventory
	UFUNCTION(BlueprintCallable, Category = "Rade")
		void SaveInventory();
	
};
