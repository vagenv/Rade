// Copyright 2015 Vagen Ayrapetyan

#pragma once

#include "GameFramework/HUD.h"
#include "Item/Inventory.h"
#include "BaseHUD.generated.h"


UCLASS()
class RADE_API ABaseHUD : public AHUD
{
	GENERATED_BODY()
public: 

	virtual void BeginPlay() override;

	// Post Begin Delay
	void PostBeginPlay();

	// Post begin Delay Time
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "")
		float PostDelay = 0.3;

	// Item index currently selected
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "")
		int32 CurrentItemSelectIndex = 0;

	// Inventory Open
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "")
		bool bInventoryOpen = false;

	// The Player pointer
	UPROPERTY()
		class ARadeCharacter* ThePlayer;

	// Get Player Function
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "The HUD")
		class ARadeCharacter* GetPlayer()const;

	// get Player health percent
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "The HUD")
		float GetPlayerHealthPercent();

	// Get Player Energy Percent
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "The HUD")
		float GetPlayerEnergyPercent();

	// Toggle Inventory Visibility
	void ToggleInventory();


	// BP Inventory Toggled
	UFUNCTION(BlueprintImplementableEvent, Category = "The HUD")
		void BP_InventoryToggled();

	// BP Scroll Up
	UFUNCTION(BlueprintImplementableEvent, Category = "The HUD")
		void BP_InventoryScrollUp();

	// BP Scroll Down
	UFUNCTION(BlueprintImplementableEvent, Category = "The HUD")
		void BP_InventoryScrollDown();

	// BP Inventory Updated
	UFUNCTION(BlueprintImplementableEvent, Category = "The HUD")
		void BP_InventoryUpdated();

	// BP Weapon Updated
	UFUNCTION(BlueprintImplementableEvent, Category = "The HUD")
		void BP_WeaponUpdated();
	
};
