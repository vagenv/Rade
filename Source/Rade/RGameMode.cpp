// Copyright 2015-2023 Vagen Ayrapetyan

#include "RGameMode.h"
#include "RSaveLib/RSaveMgr.h"

ARGameMode::ARGameMode()
{
   SaveMgr = CreateDefaultSubobject<URSaveMgr> (TEXT ("SaveMgr"));
}

void ARGameMode::BeginPlay ()
{
   Super::BeginPlay ();
}

