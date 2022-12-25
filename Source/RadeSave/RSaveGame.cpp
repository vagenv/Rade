// Copyright 2015-2022 Vagen Ayrapetyan

#include "RSaveGame.h"

URSaveGame::URSaveGame(const class FObjectInitializer& PCIP)
   : Super(PCIP)
{
   SaveSlotName = TEXT("RadeSaveSlot");
   UserIndex    = 0;
}
