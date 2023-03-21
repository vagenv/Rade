// Copyright 2015-2023 Vagen Ayrapetyan

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

float URUtilLibrary::GetRuntimeFloatCurveValue (const FRuntimeFloatCurve& InCurve, float InTime)
{
	// If an external curve is provided, we'll use that to evaluate
	if (InCurve.ExternalCurve != nullptr) {
		return InCurve.ExternalCurve->GetFloatValue (InTime);
	}

	// If no external curves are provided, we'll evaluate from the rich curve directly
	return InCurve.GetRichCurveConst ()->Eval (InTime);
}

