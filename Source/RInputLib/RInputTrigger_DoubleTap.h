// Copyright 2015-2023 Vagen Ayrapetyan

#pragma once

#include "CoreMinimal.h"
#include "InputTriggers.h"
#include "RInputTrigger_DoubleTap.generated.h"

class UEnhancedPlayerInput;

UCLASS(NotBlueprintable, meta = (DisplayName = "Double Tap", NotInputConfigurable = "true"))
class RINPUTLIB_API URInputTrigger_DoubleTap : public UInputTrigger
{
   GENERATED_BODY()

protected:
   virtual ETriggerState UpdateState_Implementation (const UEnhancedPlayerInput* PlayerInput,
                                                     FInputActionValue ModifiedValue,
                                                     float DeltaTime) override;
public:

   UPROPERTY(EditInstanceOnly, BlueprintReadWrite, Category = "Rade|Input", meta = (DisplayThumbnail = "false"))
      float Delay = .5f;

private:
   float LastTappedTime = 0.f;
};

