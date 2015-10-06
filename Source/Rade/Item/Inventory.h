// Copyright 2015 Vagen Ayrapetyan
#pragma once

#include "GameFramework/Actor.h"
#include "RadeData.h"
#include "Inventory.generated.h"

//UFUNCTION(BlueprintCallable, BlueprintPure, Category = "The HUD")
UCLASS()
class RADE_API AInventory : public AActor
{
	GENERATED_BODY()
public:	

	// Base events
	AInventory(const class FObjectInitializer& PCIP);
	virtual void BeginPlay() override;

	// Main Refs
	UPROPERTY(Replicated)
		class ARadeCharacter* ThePlayer;

	UPROPERTY()
		class ARadeGameMode* TheGM;

	// Main Data

	UPROPERTY(ReplicatedUsing = ClientReceiveUpdatedItemList, EditAnywhere, BlueprintReadWrite, Category = Inventory)
		TArray<FItemData> Items;

	UFUNCTION()
		void ClientReceiveUpdatedItemList();

	UPROPERTY(Replicated, EditAnywhere, BlueprintReadOnly)
		float TotalWeight;

	UFUNCTION(BlueprintCallable, Category = "Item")
		void UpdateInfo();

	UFUNCTION(BlueprintCallable, Category = "Item")
		void AddNewItem(TSubclassOf<AItem> NewItem);

	FItemData* AddItem(TSubclassOf<AItem> NewItem);


	UFUNCTION(BlueprintCallable, Category = "Item")
		void ActionIndex(int32 index);

	/*
	UFUNCTION(Reliable, Server, WithValidation,BlueprintCallable, Category = "Item")
	void ActionIndex(int32 index);
	virtual bool ActionIndex_Validate(int32 index);
	virtual void ActionIndex_Implementation(int32 index);
	*/


	void ItemPickedUp(class AItemPickup* ThePickup);


	UFUNCTION(BlueprintCallable, Category = "Item")
		void Action(AItem* ItemRef);

	UFUNCTION(BlueprintCallable, Category = "Item")
		void ThrowOutIndex(int32 index);
	UFUNCTION(BlueprintCallable, Category = "Item")
		void ThrowOut(AItem* ItemRef);

	UFUNCTION(BlueprintCallable, Category = "Item")
		void RemoveItemIndex(int32 ItemNumber);
	UFUNCTION(BlueprintCallable, Category = "Item")
		void RemoveItem(AItem* DeleteItem);


	UFUNCTION(BlueprintCallable, Category = "Save")
		void LoadInventory();

	UFUNCTION(BlueprintCallable, Category = "Save")
		void SaveInventory();
	
};
