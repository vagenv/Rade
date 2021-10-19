// Copyright 2015-2021 Vagen Ayrapetyan

#include "SystemSaveGame.h"
#include "../Rade.h"

USystemSaveGame::USystemSaveGame(const class FObjectInitializer& PCIP)
   : Super(PCIP)
{
   SaveSlotName = TEXT("RadeSaveSlot");
   UserIndex = 0;
}
