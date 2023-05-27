// Copyright 2015-2023 Vagen Ayrapetyan

#include "RGameInstance.h"
#include "Kismet/GameplayStatics.h"
#include "RUtilLib/RCheck.h"
#include "RUtilLib/RLog.h"

const FName RSessionName ("RadeSessionName");

FRAvaiableSessionsData::FRAvaiableSessionsData ()
{
}

FRAvaiableSessionsData::FRAvaiableSessionsData (const FOnlineSessionSearchResult &newSessionData)
{
   SessionData                 = FOnlineSessionSearchResult (newSessionData);
   OwnerName                   = newSessionData.Session.OwningUserName;
   Ping                        = newSessionData.PingInMs;
   NumberOfConnections         = newSessionData.Session.SessionSettings.NumPublicConnections;
   NumberOfAvaiableConnections = NumberOfConnections - newSessionData.Session.NumOpenPublicConnections;
}

URGameInstance::URGameInstance ()
{
   // Bind function for CREATING a Session
   OnCreateSessionCompleteDelegate = FOnCreateSessionCompleteDelegate::CreateUObject(this, &URGameInstance::OnCreateSessionComplete);
   OnStartSessionCompleteDelegate = FOnStartSessionCompleteDelegate::CreateUObject(this, &URGameInstance::OnStartOnlineGameComplete);

   // Bind function for FINDING a Session
   OnFindSessionsCompleteDelegate = FOnFindSessionsCompleteDelegate::CreateUObject(this, &URGameInstance::OnFindSessionsComplete);

   // Bind function for JOINING a Session
   OnJoinSessionCompleteDelegate = FOnJoinSessionCompleteDelegate::CreateUObject(this, &URGameInstance::OnJoinSessionComplete);

   // Bind function for DESTROYING a Session
   OnDestroySessionCompleteDelegate = FOnDestroySessionCompleteDelegate::CreateUObject(this, &URGameInstance::OnDestroySessionComplete);
}

void URGameInstance::StartOnlineGameMap (FString MapName, int32 MaxPlayerNumber)
{
   ULocalPlayer* const Player = GetFirstGamePlayer ();
   if (!ensure (Player)) return;

   // Call our custom HostSession function. GameSessionName is a GameInstance variable
   HostSession (Player->GetPreferredUniqueNetId ().GetUniqueNetId (),
                RSessionName,
                MapName,
                true,
                true,
                MaxPlayerNumber);
}

void URGameInstance::FindOnlineGames ()
{
   ULocalPlayer* const Player = GetFirstGamePlayer ();
   if (!ensure (Player)) return;

   FindSessions (Player->GetPreferredUniqueNetId ().GetUniqueNetId (),
                 RSessionName,
                 true,
                 true);
}

void URGameInstance::UpdateSessionList ()
{
   if (!SessionSearch.IsValid ()) return;

   CurrentSessionSearch.Empty ();
   for (const FOnlineSessionSearchResult &ItResult : SessionSearch->SearchResults) {
      CurrentSessionSearch.Add (FRAvaiableSessionsData (ItResult));
   }
   if (R_IS_VALID_WORLD && OnSessionListUpdated.IsBound ()) OnSessionListUpdated.Broadcast ();
}

void URGameInstance::JoinOnlineGame ()
{
   if (!SessionSearch.IsValid ())            return;
   if (!SessionSearch->SearchResults.Num ()) return;

   ULocalPlayer* const Player = GetFirstGamePlayer();

   for (const FOnlineSessionSearchResult &ItResult : SessionSearch->SearchResults) {
      if (ItResult.Session.OwningUserId != Player->GetPreferredUniqueNetId ()) {
         JoinSession (Player->GetPreferredUniqueNetId ().GetUniqueNetId (),
                      RSessionName,
                      ItResult);
         break;
      }
   }
}

// Join Selected Online Session
void URGameInstance::JoinSelectedOnlineGame (FRAvaiableSessionsData SessionData)
{
   ULocalPlayer* const Player = GetFirstGamePlayer ();
   if (!ensure (Player)) return;

   JoinSession (Player->GetPreferredUniqueNetId ().GetUniqueNetId (),
                RSessionName,
                SessionData.SessionData);
}

// Destroy Session
void URGameInstance::DestroySessionAndLeaveGame ()
{
   IOnlineSubsystem* const OnlineSub = IOnlineSubsystem::Get ();
   if (!ensure (OnlineSub)) return;

   IOnlineSessionPtr Sessions = OnlineSub->GetSessionInterface ();
   if (!Sessions.IsValid ()) {
      R_LOG ("Invalid session interface.");
      return;
   }

   Sessions->AddOnDestroySessionCompleteDelegate_Handle (OnDestroySessionCompleteDelegate);
   Sessions->DestroySession (RSessionName);
}

//=============================================================================
//                      Internal Code
//=============================================================================

bool URGameInstance::HostSession (
   TSharedPtr<const FUniqueNetId> UserId,
   FName                          SessionName,
   FString                        MapName,
   bool                           bIsLAN,
   bool                           bIsPresence,
   int32                          MaxNumPlayers)
{
   if (!UserId.IsValid()) {
      R_LOG ("Invalid UserId");
      return false;
   }

   IOnlineSubsystem* const OnlineSub = IOnlineSubsystem::Get ();
   if (!ensure (OnlineSub)) return false;

   IOnlineSessionPtr Sessions = OnlineSub->GetSessionInterface ();
   if (!Sessions.IsValid ()) {
      R_LOG ("Invalid session interface.");
      return false;
   }

   SessionSettings = MakeShareable(new FOnlineSessionSettings());

   SessionSettings->bIsLANMatch = bIsLAN;
   SessionSettings->bUsesPresence = bIsPresence;
   SessionSettings->NumPublicConnections = MaxNumPlayers;
   SessionSettings->NumPrivateConnections = 0;
   SessionSettings->bAllowInvites = true;
   SessionSettings->bAllowJoinInProgress = true;
   SessionSettings->bShouldAdvertise = true;
   SessionSettings->bAllowJoinViaPresence = true;
   SessionSettings->bAllowJoinViaPresenceFriendsOnly = false;

   SessionSettings->Set (SETTING_MAPNAME, MapName, EOnlineDataAdvertisementType::ViaOnlineService);

   // Set the delegate to the Handle of the SessionInterface
   OnCreateSessionCompleteDelegateHandle = Sessions->AddOnCreateSessionCompleteDelegate_Handle (OnCreateSessionCompleteDelegate);

   // Our delegate should get called when this is complete (doesn't need to be successful!)
   return Sessions->CreateSession (*UserId,
                                   SessionName,
                                   *SessionSettings);
}

void URGameInstance::OnCreateSessionComplete (FName SessionName, bool bWasSuccessful)
{
   IOnlineSubsystem* const OnlineSub = IOnlineSubsystem::Get ();
   if (!ensure (OnlineSub)) return;

   IOnlineSessionPtr Sessions = OnlineSub->GetSessionInterface ();
   if (!Sessions.IsValid ()) {
      R_LOG ("Invalid session interface.");
      return;
   }

   // Clear the SessionComplete delegate handle, since we finished this call
   Sessions->ClearOnCreateSessionCompleteDelegate_Handle (OnCreateSessionCompleteDelegateHandle);
   if (!bWasSuccessful) return;


   // Set the StartSession delegate handle
   OnStartSessionCompleteDelegateHandle = Sessions->AddOnStartSessionCompleteDelegate_Handle(OnStartSessionCompleteDelegate);

   // Our StartSessionComplete delegate should get called after this
   Sessions->StartSession (SessionName);
}


void URGameInstance::OnStartOnlineGameComplete (FName SessionName, bool bWasSuccessful)
{
   IOnlineSubsystem* const OnlineSub = IOnlineSubsystem::Get ();
   if (!ensure (OnlineSub)) return;

   IOnlineSessionPtr Sessions = OnlineSub->GetSessionInterface ();
   if (!Sessions.IsValid ()) {
      R_LOG ("Invalid session interface.");
      return;
   }

   // Clear the delegate, since we are done with this call
   Sessions->ClearOnStartSessionCompleteDelegate_Handle (OnStartSessionCompleteDelegateHandle);

   if (!bWasSuccessful) return;

   if (!SessionSettings.IsValid ()) return;

   FString MapName;  
   if (!SessionSettings->Get (SETTING_MAPNAME, MapName)) return;

   // TODO : Notify that Session has been started and load level
   UGameplayStatics::OpenLevel (GetWorld (),
                                *MapName,
                                true,
                                "listen");
}


void URGameInstance::FindSessions (
   TSharedPtr<const FUniqueNetId> UserId,
   FName                          SessionName,
   bool                           bIsLAN,
   bool                           bIsPresence)
{

   IOnlineSubsystem* const OnlineSub = IOnlineSubsystem::Get ();
   if (!ensure (OnlineSub)) {

      // If something goes wrong, just call the Delegate Function directly with "false".
      OnFindSessionsComplete (false);
      return;
   }

   IOnlineSessionPtr Sessions = OnlineSub->GetSessionInterface ();
   if (!Sessions.IsValid ()) {
      R_LOG ("Invalid session interface.");
      return;
   }

   if (!UserId.IsValid()) {
      R_LOG ("Invalid UserId");
      return;
   }

   bIsSearchingSession = true;
      
   // Fill in all the SearchSettings, like if we are searching for a LAN game and how many results we want to have!
   SessionSearch = MakeShareable(new FOnlineSessionSearch());

   SessionSearch->bIsLanQuery = bIsLAN;
   SessionSearch->MaxSearchResults = 20;
   SessionSearch->PingBucketSize = 50;

   // We only want to set this Query Setting if "bIsPresence" is true
   if (bIsPresence) {
      SessionSearch->QuerySettings.Set(SEARCH_PRESENCE, bIsPresence, EOnlineComparisonOp::Equals);
   }

   TSharedRef<FOnlineSessionSearch> SearchSettingsRef = SessionSearch.ToSharedRef ();

   // Set the Delegate to the Delegate Handle of the FindSession function
   OnFindSessionsCompleteDelegateHandle = Sessions->AddOnFindSessionsCompleteDelegate_Handle (OnFindSessionsCompleteDelegate);

   // Finally call the SessionInterface function. The Delegate gets called once this is finished
   Sessions->FindSessions (*UserId, SearchSettingsRef);
}

void URGameInstance::OnFindSessionsComplete (bool bWasSuccessful)
{
   bIsSearchingSession = false;

   IOnlineSubsystem* const OnlineSub = IOnlineSubsystem::Get ();
   if (!ensure (OnlineSub)) return;

   IOnlineSessionPtr Sessions = OnlineSub->GetSessionInterface ();
   if (!Sessions.IsValid ()) {
      R_LOG ("Invalid session interface.");
      return;
   }

   // Clear the Delegate handle, since we finished this call
   Sessions->ClearOnFindSessionsCompleteDelegate_Handle (OnFindSessionsCompleteDelegateHandle);

   UpdateSessionList ();
}

bool URGameInstance::JoinSession (
   TSharedPtr<const FUniqueNetId>    UserId,
   FName                             SessionName,
   const FOnlineSessionSearchResult& SearchResult)
{
   if (!UserId.IsValid ()) {
      R_LOG ("Invalid UserId");
      return false;
   }

   IOnlineSubsystem* const OnlineSub = IOnlineSubsystem::Get ();
   if (!ensure (OnlineSub)) return false;

   IOnlineSessionPtr Sessions = OnlineSub->GetSessionInterface ();
   if (!Sessions.IsValid ()) {
      R_LOG ("Invalid session interface.");
      return false;
   }

   // Set the Handle again
   OnJoinSessionCompleteDelegateHandle = Sessions->AddOnJoinSessionCompleteDelegate_Handle (OnJoinSessionCompleteDelegate);

   // Call the "JoinSession" Function with the passed "SearchResult". The "SessionSearch->SearchResults" can be used to get such a
   // "FOnlineSessionSearchResult" and pass it. Pretty straight forward!
   return Sessions->JoinSession (*UserId,
                                 SessionName,
                                 SearchResult);
}

void URGameInstance::OnJoinSessionComplete (FName SessionName, EOnJoinSessionCompleteResult::Type Result)
{
   IOnlineSubsystem* const OnlineSub = IOnlineSubsystem::Get ();
   if (!ensure (OnlineSub)) return;

   IOnlineSessionPtr Sessions = OnlineSub->GetSessionInterface ();
   if (!Sessions.IsValid ()) {
      R_LOG ("Invalid session interface.");
      return;
   }

   // Clear the Delegate again
   Sessions->ClearOnJoinSessionCompleteDelegate_Handle (OnJoinSessionCompleteDelegateHandle);

   // Get the first local PlayerController, so we can call "ClientTravel" to get to the Server Map
   // This is something the Blueprint Node "Join Session" does automatically!
   APlayerController * const PlayerController = GetFirstLocalPlayerController ();

   // We need a FString to use ClientTravel and we can let the SessionInterface construct such a
   // String for us by giving him the SessionName and an empty String. We want to do this, because
   // Every OnlineSubsystem uses different TravelURLs
   FString TravelURL;

   if (PlayerController && Sessions->GetResolvedConnectString (SessionName, TravelURL)) {
      // Finally call the ClienTravel. If you want, you could print the TravelURL to see
      // how it really looks like
      PlayerController->ClientTravel (TravelURL, ETravelType::TRAVEL_Absolute);
   }
}

// Destroy Session And return to Start Map
void URGameInstance::OnDestroySessionComplete (FName SessionName, bool bWasSuccessful)
{
   IOnlineSubsystem* const OnlineSub = IOnlineSubsystem::Get ();
   if (!ensure (OnlineSub)) return;

   IOnlineSessionPtr Sessions = OnlineSub->GetSessionInterface ();
   if (!Sessions.IsValid ()) {
      R_LOG ("Invalid session interface.");
      return;
   }

   // Clear the Delegate
   Sessions->ClearOnDestroySessionCompleteDelegate_Handle (OnDestroySessionCompleteDelegateHandle);

   if (!bWasSuccessful) return;

   // TODO : Notify that Session ended and return to main screen.
   //UGameplayStatics::OpenLevel (GetWorld (), "SelectMap", true);
}

