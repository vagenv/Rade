#pragma once

#include "CoreMinimal.h"

UENUM(BlueprintType)
enum class ERActionResult : uint8
{
   Success UMETA (DisplayName = "Success"),
   Failure UMETA (DisplayName = "Failure")
};

