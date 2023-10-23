// Copyright 2015-2023 Vagen Ayrapetyan

#include "RWorldAssetMgr.h"
#include "RUtilLib/RUtil.h"
#include "RUtilLib/RCheck.h"
#include "RUtilLib/RLog.h"

void URWorldAssetMgr::LoadAsync (const FSoftObjectPath &Path,
                                 const UObject         *Requester,
                                 TFunction<void(UObject *LoadedContent)>&& Callback)
{
   // --- Validate request
   if (!ensure (!Path.IsNull ())) return;
   if (!ensure (Requester)) return;
   URWorldAssetMgr *WorldAssetMgr = URWorldAssetMgr::GetInstance (Requester);
   if (!ensure (WorldAssetMgr)) return;

   FString RequestKey = Path.ToString () + "_" + FString::FromInt (Requester->GetUniqueID ());

   TSharedPtr<FStreamableHandle> Handle = WorldAssetMgr->StreamableManager.RequestAsyncLoad (
   Path, [RequestKey, WorldAssetMgr, Callback] () {

      if (!IsValid (WorldAssetMgr)) {
         R_LOG_STATIC ("World asset manager became invalid during load of asset.");
         return;
      }

      if (!WorldAssetMgr->Requests.Contains (RequestKey)) {
         R_LOG_STATIC_PRINTF ("[%s] not found in World asset manager request list", *RequestKey);
         return;
      }

      const RLoadAsyncRequest &Request = WorldAssetMgr->Requests[RequestKey];

      if (Request.Handle.IsValid ()) {

         if (Request.Handle->HasLoadCompleted ()) {
            if (UObject* LoadedAsset = Request.Handle->GetLoadedAsset ()) {
               if (IsValid (Request.Requester)) {
                  Callback (LoadedAsset);
               } else {
                  R_LOG_STATIC_PRINTF ("[%s] requester object is invalid", *RequestKey);
               }

            } else {
               R_LOG_STATIC_PRINTF ("[%s] loaded asset is invalid", *RequestKey);
            }
         } else {
            R_LOG_STATIC_PRINTF ("[%s] request was not completed", *RequestKey);
         }

      } else {
         R_LOG_STATIC_PRINTF ("[%s] request handle is invalid", *RequestKey);
      }

      // Not required as destructor will implicitly perform release
      // // Release data from memory
      // Request.Handle->ReleaseHandle ();
      // Request.Handle.Reset ();

      // Remove access key
      WorldAssetMgr->Requests.Remove (RequestKey);
   });

   RLoadAsyncRequest Request;
   Request.Handle    = Handle;
   Request.Path      = Path;
   Request.Requester = Requester;

   WorldAssetMgr->Requests.Add (RequestKey, Request);
}

//=============================================================================
//                   Static calls
//=============================================================================

URWorldAssetMgr* URWorldAssetMgr::GetInstance (const UObject* WorldContextObject)
{
   return URUtil::GetWorldInstance<URWorldAssetMgr> (WorldContextObject);
}

