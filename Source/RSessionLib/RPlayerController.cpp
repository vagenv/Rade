// Copyright 2015-2023 Vagen Ayrapetyan

#include "RPlayerController.h"

void ARPlayerController::PawnLeavingGame ()
{
	if (GetPawn () != NULL && DestroyPawnThenUnpossess) {
		GetPawn ()->Destroy ();
		SetPawn (NULL);
	}
}

