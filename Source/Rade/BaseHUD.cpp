// Copyright 2015 Vagen Ayrapetyan

#include "Rade.h"
#include "BaseHUD.h"
#include "Character/RadePlayer.h"


// Begin Play
void ABaseHUD::BeginPlay()
{
	Super::BeginPlay();

	// Create Delayed Post Begin Play
	FTimerHandle MyHandle;
	GetWorldTimerManager().SetTimer(MyHandle, this, &ABaseHUD::PostBeginPlay, PostDelay, false);

	// Start With Inventory CLosed
	bInventoryOpen = false;

	// Get Player Ref and Set HUD Ref in Player
	if (GetOwningPawn() && Cast<ARadePlayer>(GetOwningPawn()))
	{
		ThePlayer = Cast<ARadePlayer>(GetOwningPawn());
		Cast<ARadePlayer>(GetOwningPawn())->TheHUD = this;
	}

}

void ABaseHUD::PostBeginPlay()
{
	if (!ThePlayer && GetOwningPawn() && Cast<ARadePlayer>(GetOwningPawn()))
	{
		ThePlayer = Cast<ARadePlayer>(GetOwningPawn());
		Cast<ARadePlayer>(GetOwningPawn())->TheHUD = this;
	}
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//								Player Events


// Get Player Ref
ARadePlayer* ABaseHUD::GetPlayer()const{
	return (ThePlayer) ? ThePlayer : NULL;
}

// Get Player Health Percent
float ABaseHUD::GetPlayerHealthPercent()
{
	if (ThePlayer && ThePlayer->MaxHealth != 0 && ThePlayer->Health>0)
		return ThePlayer->Health / ThePlayer->MaxHealth;

	else return 0;
}

// Get Player Energy Percent
float ABaseHUD::GetPlayerEnergyPercent()
{
	if (ThePlayer)
		return ThePlayer->JumpJetPack.CurrentChargePercent / 100;
	else return 0;
}


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//								Inventory Events

// Toggle Inventory Visibility
void ABaseHUD::ToggleInventory()
{
	bInventoryOpen = !bInventoryOpen;
	BP_InventoryUpdated();
	BP_InventoryToggled();
}