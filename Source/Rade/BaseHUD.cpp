// Copyright 2015-2021 Vagen Ayrapetyan

#include "BaseHUD.h"
#include "Rade.h"
#include "Character/RadePlayer.h"


// Begin Play
void ABaseHUD::BeginPlay()
{
   Super::BeginPlay();

   // Start With Inventory CLosed
   bInventoryOpen = false;
   ResetReferences();
}

void ABaseHUD::ResetReferences()
{
   if (!ThePlayer) {
      if (GetOwningPawn() && Cast<ARadePlayer>(GetOwningPawn())) {
         ThePlayer = Cast<ARadePlayer>(GetOwningPawn());
         ThePlayer->TheHUD = this;
      } else {
         FTimerHandle MyHandle;
         GetWorldTimerManager().SetTimer(MyHandle, this, &ABaseHUD::ResetReferences, 0.5f, false);
      }
   }
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//                        Player Events

// Get Player Ref
ARadePlayer* ABaseHUD::GetPlayer()const{
   return (ThePlayer) ? ThePlayer : NULL;
}

// Get Player Health Percent
float ABaseHUD::GetPlayerHealthPercent()
{
   if (ThePlayer && ThePlayer->MaxHealth != 0 && ThePlayer->Health>0)
      return ThePlayer->Health / ThePlayer->MaxHealth;
   else
      return 0;
}

// Get Player Energy Percent
float ABaseHUD::GetPlayerEnergyPercent()
{
   if (ThePlayer) return ThePlayer->JumpJetPack.CurrentChargePercent / 100;
   else            return 0;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//                        Inventory Events

// Toggle Inventory Visibility
void ABaseHUD::ToggleInventory()
{
   bInventoryOpen = !bInventoryOpen;
   BP_InventoryUpdated();
   BP_InventoryToggled();
}