// Copyright 2015-2023 Vagen Ayrapetyan

#pragma once

#include "Engine/GameInstance.h"
#include "Online.h"
#include "OnlineSessionSettings.h"
#include "RGameInstance.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE (FRSessionEvent);

class FOnlineSessionSearch;

// ============================================================================
//          Available session information
// ============================================================================

USTRUCT(BlueprintType)
struct FRAvaiableSessionsData
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

// ============================================================================
//          R Game Instance
// ============================================================================

UCLASS(Blueprintable, ClassGroup=(_Rade))
class RSESSIONMANAGERLIB_API URGameInstance : public UGameInstance
{
   GENERATED_BODY()
public:

   URGameInstance ();

   //==========================================================================
   //               Params
   //==========================================================================
protected:
   // Session search manager
   TSharedPtr<FOnlineSessionSearch> SessionSearch;

   // Session Setting
   TSharedPtr<FOnlineSessionSettings> SessionSettings;

   //==========================================================================
   //               Search Session
   //==========================================================================
public:

   UFUNCTION(BlueprintCallable, Category = "Rade|Network")
      bool GetSessionList (TArray<FRAvaiableSessionsData> &Result) const;

   // Delegate when Item list updated
   UPROPERTY(BlueprintAssignable, Category = "Rade|Network")
      FRSessionEvent OnSessionListUpdated;


   // Find all available Online Sessions
   UFUNCTION(BlueprintCallable, Category = "Rade|Network")
      void FindSessions ();

   // --- Find Sessions
   // @param UserId user that initiated the request
   // @param SessionName name of session this search will generate
   // @param bIsLAN are we searching LAN matches
   // @param bIsPresence are we searching presence sessions
   void FindSessions (TSharedPtr<const FUniqueNetId> UserId, FName SessionName, bool bIsLAN, bool bIsPresence);


   // --- Find Session Complete
   void OnFindSessionsComplete (bool bWasSuccessful);

protected:
   // Found
   FOnFindSessionsCompleteDelegate OnFindSessionsCompleteDelegate;
   FDelegateHandle                 OnFindSessionsCompleteDelegateHandle;

   //==========================================================================
   //                   Start Session
   //==========================================================================
public:
   // Host Specific Online Map Session
   UFUNCTION(BlueprintCallable, Category = "Rade|Network")
      void StartOnlineGameMap (FString MapName, int32 MaxPlayerNumber = 16);


   // --- Host A Game
   // @Param      UserID         User that started the request
   // @Param      SessionName    Name of the Session
   // @Param      bIsLAN         Is this is LAN Game?
   // @Param      bIsPresence    "Is the Session to create a presence Session"
   // @Param      MaxNumPlayers
   bool HostSession (TSharedPtr<const FUniqueNetId> UserId, FName SessionName, FString MapName, bool bIsLAN, bool bIsPresence, int32 MaxNumPlayers);

   // --- Create Session Complete
   void OnCreateSessionComplete (FName SessionName, bool bWasSuccessful);

   // --- Start Online Game Complete
   void OnStartOnlineGameComplete (FName SessionName, bool bWasSuccessful);
   
protected:
   // Created
   FOnCreateSessionCompleteDelegate OnCreateSessionCompleteDelegate;
   FDelegateHandle                  OnCreateSessionCompleteDelegateHandle;

   // Started
   FOnStartSessionCompleteDelegate OnStartSessionCompleteDelegate;
   FDelegateHandle                 OnStartSessionCompleteDelegateHandle;

   //==========================================================================
   //                   Join Session
   //==========================================================================
public:

   
   // Disable join methods
   virtual bool JoinSession (ULocalPlayer* LocalPlayer, const FOnlineSessionSearchResult& SearchResult) override{ return false; }
   virtual bool JoinSession (ULocalPlayer* LocalPlayer, int32 SessionIndexInSearchResults) override { return false; } 

   // Join Specific Online Session
   UFUNCTION(BlueprintCallable, Category = "Rade|Network")
      void JoinSession (FRAvaiableSessionsData SessionData);
   
   // --- Join Session
   bool JoinSession (TSharedPtr<const FUniqueNetId> UserId, FName SessionName, const FOnlineSessionSearchResult& SearchResult);

   // --- Join Session is Complete
   void OnJoinSessionComplete (FName SessionName, EOnJoinSessionCompleteResult::Type Result);

protected:
   // Joined
   FOnJoinSessionCompleteDelegate OnJoinSessionCompleteDelegate;
   FDelegateHandle                OnJoinSessionCompleteDelegateHandle;

   //==========================================================================
   //                   Leave Session
   //==========================================================================
public:
   // Stop Session and Quit Game
   UFUNCTION(BlueprintCallable, Category = "Rade|Network")
      void DestroySessionAndLeaveGame ();

   void OnDestroySessionComplete (FName SessionName, bool bWasSuccessful);

protected:
   // Destryoyed
   FOnDestroySessionCompleteDelegate OnDestroySessionCompleteDelegate;
   FDelegateHandle                   OnDestroySessionCompleteDelegateHandle;
};

