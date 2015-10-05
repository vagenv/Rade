// Copyright 2015 Vagen Ayrapetyan

#include "Rade.h"
#include "BaseHUD.h"
#include "Character/RadeCharacter.h"

void ABaseHUD::BeginPlay()
{
	Super::BeginPlay();

	FTimerHandle MyHandle;
	GetWorldTimerManager().SetTimer(MyHandle, this, &ABaseHUD::PostBeginPlay, PostDelay, false);

	bInventoryOpen = false;

	if (GetOwningPawn() && Cast<ARadeCharacter>(GetOwningPawn()))
	{
		ThePlayer = Cast<ARadeCharacter>(GetOwningPawn());
		Cast<ARadeCharacter>(GetOwningPawn())->TheHUD = this;
	}

}

void ABaseHUD::PostBeginPlay()
{
	if (!ThePlayer && GetOwningPawn() && Cast<ARadeCharacter>(GetOwningPawn()))
	{
		ThePlayer = Cast<ARadeCharacter>(GetOwningPawn());
		Cast<ARadeCharacter>(GetOwningPawn())->TheHUD = this;
	}
}

ARadeCharacter* ABaseHUD::GetPlayer()const
{
	if (ThePlayer)
		return  ThePlayer;
	else return NULL;
}


float ABaseHUD::GetPlayerHealthPercent()
{
	if (ThePlayer)
	{
		if (ThePlayer->MaxHealth!=0 && ThePlayer->Health>0)
		{
			return ThePlayer->Health / ThePlayer->MaxHealth;
		}
	}
	return 0;
}

float ABaseHUD::GetPlayerEnergyPercent()
{
	if (ThePlayer)
	{
		return ThePlayer->JumpJetPack.CurrentChargePercent / 100;
	
	}
	return 0;
}



void ABaseHUD::ToggleInventory()
{
	bInventoryOpen = !bInventoryOpen;
	BP_InventoryUpdated();
	BP_InventoryToggled();
}