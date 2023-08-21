// Copyright 2015-2023 Vagen Ayrapetyan

#include "RInputTrigger_DoubleTap.h"
#include "EnhancedPlayerInput.h"

ETriggerState URInputTrigger_DoubleTap::UpdateState_Implementation (const UEnhancedPlayerInput* PlayerInput,
                                                                   FInputActionValue ModifiedValue,
                                                                   float DeltaTime)
{
   if (   IsActuated (ModifiedValue)
      && !IsActuated (LastValue)
      && PlayerInput
      && PlayerInput->GetOuterAPlayerController ())
   {
      const float CurrentTime = PlayerInput->GetOuterAPlayerController ()->GetWorld ()->GetRealTimeSeconds ();
      if (CurrentTime - LastTappedTime < Delay) {
         LastTappedTime = 0;
         return ETriggerState::Triggered;
      }
      LastTappedTime = CurrentTime;
   }
   return ETriggerState::None;
}

