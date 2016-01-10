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


	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	//								Local Player Properties Properties and Events

	// The Player pointer
	UPROPERTY()
		class ARadePlayer* ThePlayer;

	// Get Player pointer
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "The HUD")
		class ARadePlayer* GetPlayer()const;

	// Get Player Health percent
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "The HUD")
		float GetPlayerHealthPercent();

	// Get Player Energy Percent
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "The HUD")
		float GetPlayerEnergyPercent();

	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	//								Inventory Properties and Events

	// Toggle Inventory Visibility
	void ToggleInventory();

	// Item index currently selected
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "")
		int32 CurrentItemSelectIndex = 0;

	// Is Inventory Open ?
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "")
		bool bInventoryOpen = false;


	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	//								Blueprint Events


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
