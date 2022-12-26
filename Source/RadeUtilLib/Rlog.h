#pragma once

#include "Engine.h"


RADEUTIL_API DECLARE_LOG_CATEGORY_EXTERN(RadeLog, Log, All);

///////////////////////////////////////////////////////////////////////////////

//                        Custom Print Methods

static FString DxLog_GetAuthStr (UObject* WorldContext) {
   if (!ensure (WorldContext)) return "";
   UWorld* World = WorldContext->GetWorld ();
   if (!ensure (World)) return "";
   return World->IsNetMode (NM_Client) ? "[CLIENT] " : "[SERVER] ";
}

#define rlog(text) UE_LOG (RadeLog, \
                           Log, \
                           TEXT("%s"), \
                           *(DxLog_GetAuthStr (this) + __FUNCTION__ + FString (": ") + text));
#define rprint(text) if (GEngine) GEngine->AddOnScreenDebugMessage (-1, 3, FColor::White, \
                             DxLog_GetAuthStr (this) + __FUNCTION__ + FString (": ") + text);

///////////////////////////////////////////////////////////////////////////////

//                        Custom Load Object from Path

/*
//TEMPLATE Load Obj From Path
template <typename ObjClass>
static ObjClass* LoadObjFromPath(const FName& Path)
{
   if (Path == NAME_None) return NULL;
   return Cast<ObjClass>(StaticLoadObject(ObjClass::StaticClass(), NULL, *Path.ToString()));
}
*/