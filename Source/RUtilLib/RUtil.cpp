#include "RUtil.h"

ACharacter* URUtilLibrary::GetLocalRadePlayer (UObject* WorldContextObject)
{
   if (!ensure (WorldContextObject)) return nullptr;
   UWorld* World = GEngine->GetWorldFromContextObject (WorldContextObject, EGetWorldErrorMode::ReturnNull);
   if (!World) return nullptr;

   ACharacter *Result = nullptr;
   if (APlayerController *localPC = World->GetFirstPlayerController ()) {
      Result = Cast<ACharacter>(localPC->GetPawn ());
   }
   return Result;
}

