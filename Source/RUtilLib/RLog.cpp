#include "RLog.h"

DEFINE_LOG_CATEGORY (RadeLog);

FString RLog_GetAuthStr (const UObject* WorldContext) {
   if (!ensure (WorldContext != nullptr)) return "[STATIC]";
   UWorld* World = WorldContext->GetWorld ();
   if (!ensure (World != nullptr)) return "[No World]";
   return World->IsNetMode (NM_Client) ? "[CLIENT] " : "[SERVER] ";
}

void __rlog_raw (const FString &msg)
{
   UE_LOG (RadeLog, Log, TEXT ("%s"), *msg);
}

void __rprint_raw (const FString &msg)
{
   if (GEngine) GEngine->AddOnScreenDebugMessage (-1, 3, FColor::White, msg);
}

void __rlog_internal (const UObject* WorldContext,
                      const FString &msg,
                      const char    *file,
                      int            line,
                      const char    *func)
{
   TArray<FStringFormatArg> args;
   args.Add (FStringFormatArg (RLog_GetAuthStr (WorldContext)));
   args.Add (FStringFormatArg (file));
   args.Add (FStringFormatArg (line));
   args.Add (FStringFormatArg (func));
   args.Add (FStringFormatArg (msg));
   FString res = FString::Format (TEXT ("{0}[{1} +{2}] {3}: {4}"), args);

   __rlog_raw   (res);
   __rprint_raw (res);
}

void __rprint_internal (const UObject* WorldContext,
                        const FString &msg,
                        const char    *file,
                        int            line,
                        const char    *func)
{
   TArray<FStringFormatArg> args;
   args.Add (FStringFormatArg (RLog_GetAuthStr (WorldContext)));
   args.Add (FStringFormatArg (file));
   args.Add (FStringFormatArg (line));
   args.Add (FStringFormatArg (func));
   args.Add (FStringFormatArg (msg));
   FString res = FString::Format (TEXT ("[{0}][{1} +{2}] {3}: {4}"), args);

   __rprint_raw (res);
}

