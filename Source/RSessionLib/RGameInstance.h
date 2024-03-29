// Copyright 2015-2023 Vagen Ayrapetyan

#pragma once

#include "Engine/GameInstance.h"
#include "Online.h"
#include "RSessionTypes.h"
#include "RGameInstance.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE (FRSessionMgrEvent);

// ============================================================================
//          R Game Instance
// ============================================================================

UCLASS(Blueprintable, ClassGroup=(_Rade))
class RSESSIONLIB_API URGameInstance : public UGameInstance
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

   UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Rade|Network")
      TSoftObjectPtr<UWorld> DefaultLevel;

private:
   // Level before client connection
   FString LastLevelMap;

   //==========================================================================
   //               Session Status
   //==========================================================================
public:

   UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Rade|Network")
      bool HasSession () const;

   UFUNCTION(BlueprintCallable, Category = "Rade|Network")
      void ResetSession ();

   //==========================================================================
   //               Search Session
   //==========================================================================
public:

   UFUNCTION(BlueprintCallable, Category = "Rade|Network")
      bool GetSessionList (TArray<FRAvaiableSessionsData> &Result) const;

   // Delegate when session list updated
   UPROPERTY(BlueprintAssignable, Category = "Rade|Network")
      FRSessionMgrEvent OnSessionListUpdated;


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
      void HostSession (int32 MaxPlayerNumber = 16);

   // --- Host A Game
   // @Param      UserID         User that started the request
   // @Param      SessionName    Name of the Session
   // @Param      bIsLAN         Is this is LAN Game?
   // @Param      bIsPresence    "Is the Session to create a presence Session"
   // @Param      MaxNumPlayers
   bool HostSession (TSharedPtr<const FUniqueNetId> UserId,
                     FName                          SessionName,
                     bool                           bIsLAN,
                     bool                           bIsPresence,
                     int32                          MaxNumPlayers);

   // --- Create Session Complete
   void OnCreateSessionComplete (FName SessionName, bool bWasSuccessful);

   // --- Start Online Game Complete
   void OnStartSessionComplete (FName SessionName, bool bWasSuccessful);

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
      bool JoinSession (FRAvaiableSessionsData SessionData);

   // --- Join Session
   bool JoinSession (TSharedPtr<const FUniqueNetId> UserId, FName SessionName, const FOnlineSessionSearchResult& SearchResult);

   // --- Join Session is Complete
   void OnJoinSessionComplete (FName SessionName, EOnJoinSessionCompleteResult::Type Result);

   // Delegate load of session failed
   UPROPERTY(BlueprintAssignable, Category = "Rade|Network")
      FRSessionMgrEvent OnJoinSessionError;

protected:
   // Joined
   FOnJoinSessionCompleteDelegate OnJoinSessionCompleteDelegate;
   FDelegateHandle                OnJoinSessionCompleteDelegateHandle;

   //==========================================================================
   //                   Kick Player
   //==========================================================================

   // Kick specific player
   UFUNCTION(BlueprintCallable, Category = "Rade|Network")
      bool KickPlayer (APlayerController* KickedPlayer);


   //==========================================================================
   //                   Leave Session
   //==========================================================================
public:

   // Stop Session and Quit Game
   UFUNCTION(BlueprintCallable, Category = "Rade|Network")
      bool LeaveSession ();

   void OnDestroySessionComplete (FName SessionName, bool bWasSuccessful);

protected:
   // Destryoyed
   FOnDestroySessionCompleteDelegate OnDestroySessionCompleteDelegate;
   FDelegateHandle                   OnDestroySessionCompleteDelegateHandle;
};

