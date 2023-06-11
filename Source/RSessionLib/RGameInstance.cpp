// Copyright 2015-2023 Vagen Ayrapetyan

#include "RGameInstance.h"
#include "RUtilLib/RCheck.h"
#include "RUtilLib/RLog.h"

#include "Kismet/GameplayStatics.h"
#include "Online/OnlineSessionNames.h"

const FName RSessionName ("RSession");

// ============================================================================
//          R Game Instance
// ============================================================================

URGameInstance::URGameInstance ()
{
   // Bind function for FINDING a Session
   OnFindSessionsCompleteDelegate = FOnFindSessionsCompleteDelegate::CreateUObject (
      this, &URGameInstance::OnFindSessionsComplete);

   // Bind function for CREATING a Session
   OnCreateSessionCompleteDelegate = FOnCreateSessionCompleteDelegate::CreateUObject (
      this, &URGameInstance::OnCreateSessionComplete);
   OnStartSessionCompleteDelegate = FOnStartSessionCompleteDelegate::CreateUObject (
      this, &URGameInstance::OnStartSessionComplete);

   // Bind function for JOINING a Session
   OnJoinSessionCompleteDelegate = FOnJoinSessionCompleteDelegate::CreateUObject (
      this, &URGameInstance::OnJoinSessionComplete);

   // Bind function for DESTROYING a Session
   OnDestroySessionCompleteDelegate = FOnDestroySessionCompleteDelegate::CreateUObject (
      this, &URGameInstance::OnDestroySessionComplete);
}

//=============================================================================
//               Session Status
//=============================================================================

bool URGameInstance::HasSession () const
{
   UWorld* World = GetWorld ();
   if (!ensure (World)) return false;

   if (World->GetNetMode () == ENetMode::NM_Client)       return true;
   if (World->GetNetMode () == ENetMode::NM_ListenServer) return true;

   // TODO: Check SessionSettings?

   return false;
}

// ============================================================================
//          Search Session list
// ============================================================================

bool URGameInstance::GetSessionList (TArray<FRAvaiableSessionsData> &Result) const
{
   if (!SessionSearch.IsValid ()) return false;
   Result.Empty ();
   for (const FOnlineSessionSearchResult &ItResult : SessionSearch->SearchResults) {
      Result.Add (FRAvaiableSessionsData (ItResult));
   }
   return true;
}

void URGameInstance::FindSessions ()
{
   ULocalPlayer* const Player = GetFirstGamePlayer ();
   if (!ensure (Player)) return;

   FindSessions (Player->GetPreferredUniqueNetId ().GetUniqueNetId (),
                 RSessionName,
                 true,
                 true);
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
      
   // Fill in all the SearchSettings, like if we are searching for a LAN game and how many results we want to have!
   SessionSearch = MakeShareable(new FOnlineSessionSearch());

   SessionSearch->bIsLanQuery = bIsLAN;
   SessionSearch->MaxSearchResults = 32;
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
   IOnlineSubsystem* const OnlineSub = IOnlineSubsystem::Get ();
   if (!ensure (OnlineSub)) return;

   IOnlineSessionPtr Sessions = OnlineSub->GetSessionInterface ();
   if (!Sessions.IsValid ()) {
      R_LOG ("Invalid session interface.");
      return;
   }

   // Clear the Delegate handle, since we finished this call
   Sessions->ClearOnFindSessionsCompleteDelegate_Handle (OnFindSessionsCompleteDelegateHandle);

   if (R_IS_VALID_WORLD && OnSessionListUpdated.IsBound ()) OnSessionListUpdated.Broadcast ();
}


// ============================================================================
//          Start Session
// ============================================================================

void URGameInstance::HostSession (int32 MaxPlayerNumber)
{
   if (HasSession ()) {
      R_LOG ("Already in session");
      return;
   }

   ULocalPlayer* const Player = GetFirstGamePlayer ();
   if (!ensure (Player)) return;

   // Call our custom HostSession function. GameSessionName is a GameInstance variable
   HostSession (Player->GetPreferredUniqueNetId ().GetUniqueNetId (),
                RSessionName,
                true,
                true,
                MaxPlayerNumber);
}

bool URGameInstance::HostSession (
   TSharedPtr<const FUniqueNetId> UserId,
   FName                          SessionName,
   bool                           bIsLAN,
   bool                           bIsPresence,
   int32                          MaxNumPlayers)
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
   if (!bWasSuccessful) {
      R_LOG ("Failed to create session");
      return;
   }

   // Set the StartSession delegate handle
   OnStartSessionCompleteDelegateHandle = Sessions->AddOnStartSessionCompleteDelegate_Handle(OnStartSessionCompleteDelegate);

   // Our StartSessionComplete delegate should get called after this
   Sessions->StartSession (SessionName);
}


void URGameInstance::OnStartSessionComplete (FName SessionName, bool bWasSuccessful)
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

   if (!bWasSuccessful) {
      R_LOG ("Failed to start session");
      return;
   }

   if (!SessionSettings.IsValid ()) {
      R_LOG ("Invalid session settings");
      return;
   }

   // --- Change to listen server
   UWorld* World = GetWorld ();
   if (!ensure (World)) return;

   FURL Url (nullptr, *World->GetLocalURL (), TRAVEL_Absolute);
	World->Listen (Url);
}


// ============================================================================
//          Join Session
// ============================================================================

// Join Selected Online Session
void URGameInstance::JoinSession (FRAvaiableSessionsData SessionData)
{
    if (HasSession ()) {
      R_LOG ("Already in session");
      return;
   }

   ULocalPlayer* const Player = GetFirstGamePlayer ();
   if (!ensure (Player)) return;

   JoinSession (Player->GetPreferredUniqueNetId ().GetUniqueNetId (),
                RSessionName,
                SessionData.SessionData);
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

   // Save backup of current level
   LastLevelMap = GetWorld ()->GetPackage ()->GetPathName ();
   if (!LastLevelMap.IsEmpty ()) {
      R_LOG ("Set default map to: " + UGameMapsSettings::GetGameDefaultMap () + " => " + LastLevelMap);
      UGameMapsSettings::SetGameDefaultMap (LastLevelMap);  
   }

   // Set the Handle again
   OnJoinSessionCompleteDelegateHandle = Sessions->AddOnJoinSessionCompleteDelegate_Handle (OnJoinSessionCompleteDelegate);

   // Join
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
   APlayerController* const PlayerController = GetFirstLocalPlayerController ();


   if (!PlayerController) {
      R_LOG ("No Player controller.");
      return;
   }

   // We need a FString to use ClientTravel and we can let the SessionInterface construct such a
   // String for us by giving him the SessionName and an empty String. We want to do this, because
   // Every OnlineSubsystem uses different TravelURLs
   FString TravelURL;

   if (!Sessions->GetResolvedConnectString (SessionName, TravelURL)) {
      R_LOG ("Failed to get target TravelURL");
      return;
   }

   // Finally call the ClienTravel. If you want, you could print the TravelURL to see
   // how it really looks like
   PlayerController->ClientTravel (TravelURL, ETravelType::TRAVEL_Absolute);
}

// ============================================================================
//          Kick player
// ============================================================================

bool URGameInstance::KickPlayer (APlayerController* KickedPlayer)
{ 
   if (KickedPlayer == NULL) {
      R_LOG ("Invalid controller ptr");
      return false;
   }

   UWorld* World = GetWorld ();
   if (!ensure (World)) return false;

   if (World->GetNetMode () == ENetMode::NM_Client) {
      R_LOG ("Client can't kick players.");
      return false;
   }

   AGameModeBase *GameMode = World->GetAuthGameMode ();
   if (!GameMode) {
      R_LOG ("Invalid Game Mode object.");
      return false;
   }

   if (!IsValid (GameMode->GameSession)) {
      R_LOG ("Invalid Game Session object.");
      return false;
   }

   FText KickReason = FText::FromString (TEXT ("Server Kick"));

   return GameMode->GameSession->KickPlayer (KickedPlayer, KickReason);
}

// ============================================================================
//          Leave session
// ============================================================================

void URGameInstance::LeaveSession ()
{
   if (!HasSession ()) {
      R_LOG ("No session");
      return;
   }

   IOnlineSubsystem* const OnlineSub = IOnlineSubsystem::Get ();
   if (!ensure (OnlineSub)) return;

   IOnlineSessionPtr Sessions = OnlineSub->GetSessionInterface ();
   if (!Sessions.IsValid ()) {
      R_LOG ("Invalid session interface.");
      return;
   }

   OnDestroySessionCompleteDelegateHandle
      = Sessions->AddOnDestroySessionCompleteDelegate_Handle (OnDestroySessionCompleteDelegate);
   Sessions->DestroySession (RSessionName);
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

   if (!bWasSuccessful) {
      R_LOG ("Failed to destroy session");
      return;
   }

   UWorld* World = GetWorld ();
   if (!ensure (World)) return;

   // Client
   if (World->GetNetMode () == ENetMode::NM_Client) {

      // Return to previous level
      if (!LastLevelMap.IsEmpty ()) {
         UGameplayStatics::OpenLevel (World, *LastLevelMap);
         LastLevelMap = "";
      }
   }

   // Server
   if (World->GetNetMode () == ENetMode::NM_ListenServer) {

      // Disconnect all players
      GetEngine ()->ShutdownWorldNetDriver (World);
   }
}

