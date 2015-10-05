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
	void PostBeginPlay();

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "")
		float PostDelay = 0.3;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "")
		int32 CurrentItemSelectIndex = 0;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "")
		bool bInventoryOpen = false;

	UPROPERTY()
		class ARadeCharacter* ThePlayer;

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "The HUD")
		class ARadeCharacter* GetPlayer()const;


	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "The HUD")
		float GetPlayerHealthPercent();

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "The HUD")
		float GetPlayerEnergyPercent();

	void ToggleInventory();

	UFUNCTION(BlueprintImplementableEvent, Category = "The HUD")
		void BP_InventoryToggled();

	UFUNCTION(BlueprintImplementableEvent, Category = "The HUD")
		void BP_InventoryScrollUp();

	UFUNCTION(BlueprintImplementableEvent, Category = "The HUD")
		void BP_InventoryScrollDown();

	UFUNCTION(BlueprintImplementableEvent, Category = "The HUD")
		void BP_InventoryUpdated();


	UFUNCTION(BlueprintImplementableEvent, Category = "The HUD")
		void BP_WeaponUpdated();
	
};
