// Copyright 2015-2023 Vagen Ayrapetyan

#include "RInputTrigger_DoubleTap.h"
#include "EnhancedPlayerInput.h"
#include "RUtilLib/RUtil.h"

ETriggerState URInputTrigger_DoubleTap::UpdateState_Implementation (const UEnhancedPlayerInput* PlayerInput,
                                                                   FInputActionValue ModifiedValue,
                                                                   float DeltaTime)
{
   if (IsActuated (ModifiedValue) && !IsActuated (LastValue)) {

      UWorld* World = nullptr;
      if (PlayerInput) World = URUtil::GetWorld (PlayerInput->GetOuterAPlayerController ());

      if (World) {
         const float CurrentTime = World->GetRealTimeSeconds ();
         if (CurrentTime - LastTappedTime < Delay) {
            LastTappedTime = 0;
            return ETriggerState::Triggered;
         }
         LastTappedTime = CurrentTime;
      }
   }
   return ETriggerState::None;
}

