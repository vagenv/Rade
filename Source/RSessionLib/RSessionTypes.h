// Copyright 2015-2023 Vagen Ayrapetyan

#pragma once

#include "OnlineSessionSettings.h"
#include "RSessionTypes.generated.h"

class FOnlineSessionSearch;

// ============================================================================
//          Available session information
// ============================================================================

USTRUCT(Blueprintable, BlueprintType)
struct RSESSIONLIB_API FRAvaiableSessionsData
{
   GENERATED_BODY()

   UPROPERTY(EditAnywhere, BlueprintReadWrite)
      FString Hostname;

   UPROPERTY(EditAnywhere, BlueprintReadWrite)
      int32 Ping = 0;

   UPROPERTY(EditAnywhere, BlueprintReadWrite)
      int32 ConnectionsMax = 0;

   UPROPERTY(EditAnywhere, BlueprintReadWrite)
      int32 ConnectionsBusy = 0;

   // Keep copy
   FOnlineSessionSearchResult SessionData;

   FRAvaiableSessionsData ();
   FRAvaiableSessionsData (const FOnlineSessionSearchResult &newSessionData);
};


