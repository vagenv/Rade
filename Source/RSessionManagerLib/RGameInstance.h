// Copyright 2015-2023 Vagen Ayrapetyan

#pragma once

#include "Engine/GameInstance.h"
#include "Online.h"
#include "RGameInstance.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE (FRSessionEvent);

class FOnlineSessionSearch;

// Online Session Data Struct
USTRUCT(BlueprintType)
struct FRAvaiableSessionsData
{
   GENERATED_BODY()

   UPROPERTY(EditAnywhere, BlueprintReadWrite)
      FString OwnerName = "Not Set";

   UPROPERTY(EditAnywhere, BlueprintReadWrite)
      int32 Ping = 0;

   UPROPERTY(EditAnywhere, BlueprintReadWrite)
      int32 NumberOfConnections = 0;

   UPROPERTY(EditAnywhere, BlueprintReadWrite)
      int32 NumberOfAvaiableConnections = 0;

   // Keep copy
   FOnlineSessionSearchResult SessionData;

   FRAvaiableSessionsData ();
   FRAvaiableSessionsData (const FOnlineSessionSearchResult &newSessionData);
};

// Custom Game Instance
UCLASS(Blueprintable, ClassGroup=(_Rade))
class RSESSIONMANAGERLIB_API URGameInstance : public UGameInstance
{
   GENERATED_BODY()
public:

   URGameInstance ();

   //==========================================================================
   //               Callable Events and Functions
   //==========================================================================

   // Host Specific Online Map Session
   UFUNCTION(BlueprintCallable, Category = "Rade|Network")
      void StartOnlineGameMap (FString MapName = TEXT ("NewMap"), int32 MaxPlayerNumber = 16);

   // Find all available Online Sessions
   UFUNCTION(BlueprintCallable, Category = "Rade|Network")
      void FindOnlineGames ();

   // Update available Online Sessions
   UFUNCTION(BlueprintCallable, Category = "Rade|Network")
      void UpdateSessionList ();

   // Join Any Online Session
   UFUNCTION(BlueprintCallable, Category = "Rade|Network")
      void JoinOnlineGame ();

   // Join Specific Online Session
   UFUNCTION(BlueprintCallable, Category = "Rade|Network")
      void JoinSelectedOnlineGame (FRAvaiableSessionsData SessionData);

   // Stop Session and Quit Game
   UFUNCTION(BlueprintCallable, Category = "Rade|Network")
      void DestroySessionAndLeaveGame ();

   //==========================================================================
   //               Data about the Sessions and Game Instance
   //==========================================================================

   // Current Online Sessions Search Result
   UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Rade|Network")
      TArray<FRAvaiableSessionsData> CurrentSessionSearch;

   // Delegate when Item list updated
   UPROPERTY(BlueprintAssignable, Category = "Rade|Network")
      FRSessionEvent OnSessionListUpdated;

   // Is Currently Searching Sessions?
   UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Rade|Network")
      bool bIsSearchingSession = false;

   //==========================================================================
   //               Settings
   //==========================================================================

   // --- Variable Part for FINDING a Session
   TSharedPtr<FOnlineSessionSearch> SessionSearch;

   // Session Setting
   TSharedPtr<FOnlineSessionSettings> SessionSettings;

   //==========================================================================
   //               Delegate
   //==========================================================================

   // Delegate called when session created
   FOnCreateSessionCompleteDelegate OnCreateSessionCompleteDelegate;
   // Delegate called when session started
   FOnStartSessionCompleteDelegate OnStartSessionCompleteDelegate;

   // Handles to registered delegates for creating a session
   FDelegateHandle OnCreateSessionCompleteDelegateHandle;
   FDelegateHandle OnStartSessionCompleteDelegateHandle;

   // Delegate for searching for sessions
   FOnFindSessionsCompleteDelegate OnFindSessionsCompleteDelegate;

   // Handle to registered delegate for searching a session
   FDelegateHandle OnFindSessionsCompleteDelegateHandle;

   // ---  Variable Part for JOINING a Session

   // Delegate after joining a session
   FOnJoinSessionCompleteDelegate OnJoinSessionCompleteDelegate;

   // Handle to registered delegate for joining a session
   FDelegateHandle OnJoinSessionCompleteDelegateHandle;

   // --- Variable Part for DESTROYING a Session

   // Delegate for destroying a session
   FOnDestroySessionCompleteDelegate OnDestroySessionCompleteDelegate;

   // Handle to registered delegate for destroying a session
   FDelegateHandle OnDestroySessionCompleteDelegateHandle;

   //==========================================================================
   //                        Internal Events
   //==========================================================================

   // --- Host A Game
   // @Param      UserID         User that started the request
   // @Param      SessionName    Name of the Session
   // @Param      bIsLAN         Is this is LAN Game?
   // @Param      bIsPresence    "Is the Session to create a presence Session"
   // @Param      MaxNumPlayers
   bool HostSession (TSharedPtr<const FUniqueNetId> UserId, FName SessionName, FString MapName, bool bIsLAN, bool bIsPresence, int32 MaxNumPlayers);

   // --- Create Session Complete
   virtual void OnCreateSessionComplete (FName SessionName, bool bWasSuccessful);

   // --- Start Online Game Complete
   void OnStartOnlineGameComplete (FName SessionName, bool bWasSuccessful);

   // --- Find Sessions
   // @param UserId user that initiated the request
   // @param SessionName name of session this search will generate
   // @param bIsLAN are we searching LAN matches
   // @param bIsPresence are we searching presence sessions
   void FindSessions (TSharedPtr<const FUniqueNetId> UserId, FName SessionName, bool bIsLAN, bool bIsPresence);


   // --- Find Session Complete
   void OnFindSessionsComplete (bool bWasSuccessful);

   // --- Join Session
   bool JoinSession (TSharedPtr<const FUniqueNetId> UserId, FName SessionName, const FOnlineSessionSearchResult& SearchResult);

   // Disable default join methods
   virtual bool JoinSession (ULocalPlayer* LocalPlayer, const FOnlineSessionSearchResult& SearchResult) { return false; }
   virtual bool JoinSession (ULocalPlayer* LocalPlayer, int32 SessionIndexInSearchResults) { return false; }

   // --- Join Session is Complete
   void OnJoinSessionComplete (FName SessionName, EOnJoinSessionCompleteResult::Type Result);

   // --- DESTROYING a Session
   virtual void OnDestroySessionComplete (FName SessionName, bool bWasSuccessful);
};

