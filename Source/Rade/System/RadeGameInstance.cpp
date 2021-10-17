// Copyright 2015-2021 Vagen Ayrapetyan

#include "RadeGameInstance.h"
#include "../Rade.h"
#include "../Character/RadePlayer.h"


const FName theSessionName ("RadeSessionName");

URadeGameInstance::URadeGameInstance(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	/** Bind function for CREATING a Session */
	OnCreateSessionCompleteDelegate = FOnCreateSessionCompleteDelegate::CreateUObject(this, &URadeGameInstance::OnCreateSessionComplete);
	OnStartSessionCompleteDelegate  = FOnStartSessionCompleteDelegate::CreateUObject(this, &URadeGameInstance::OnStartOnlineGameComplete);

	/** Bind function for FINDING a Session */
	OnFindSessionsCompleteDelegate = FOnFindSessionsCompleteDelegate::CreateUObject(this, &URadeGameInstance::OnFindSessionsComplete);

	/** Bind function for JOINING a Session */
	OnJoinSessionCompleteDelegate = FOnJoinSessionCompleteDelegate::CreateUObject(this, &URadeGameInstance::OnJoinSessionComplete);

	/** Bind function for DESTROYING a Session */
	OnDestroySessionCompleteDelegate = FOnDestroySessionCompleteDelegate::CreateUObject(this, &URadeGameInstance::OnDestroySessionComplete);
}


// Set Local Player Stats
void URadeGameInstance::SetPlayerStats(FString newPlayerName, FLinearColor newPlayerColor, FVector newMaterialSettings)
{
	PlayerName             = newPlayerName;
	PlayerColor            = newPlayerColor;
	PlayerMaterialSettings = newMaterialSettings;
}

// Host Default Map
void URadeGameInstance::StartOnlineGame()
{
	// Creating a local player where we can get the UserID from
	ULocalPlayer* const Player = GetFirstGamePlayer();

	TheMapName = "BattleArena";

	// Call our custom HostSession function. GameSessionName is a GameInstance variable
   HostSession(Player->GetPreferredUniqueNetId ().GetUniqueNetId(), *PlayerName, TheMapName, true, true, 16);
}

// Host Specific Map
void URadeGameInstance::StartOnlineGameMap(FString MapName, int32 MaxPlayerNumber)
{
	// Creating a local player where we can get the UserID from
	ULocalPlayer* const Player = GetFirstGamePlayer();

	//Player->GetPreferredUniqueNetId()
	TheMapName = MapName;

	// Call our custom HostSession function. GameSessionName is a GameInstance variable
   HostSession(Player->GetPreferredUniqueNetId().GetUniqueNetId(), theSessionName, TheMapName, true, true, MaxPlayerNumber);
}

// Find All Online Sessions
void URadeGameInstance::FindOnlineGames()
{
	ULocalPlayer* const Player = GetFirstGamePlayer();
   FindSessions(Player->GetPreferredUniqueNetId().GetUniqueNetId(), theSessionName, true, true);
}

// Update Available Sessions List
void URadeGameInstance::UpdateSessionList()
{
	if (!SessionSearch.IsValid()) return;

	CurrentSessionSearch.Empty();
	for (int32 i = 0; i < SessionSearch->SearchResults.Num(); i++) {
		FAvaiableSessionsData NewData(SessionSearch->SearchResults[i]);
		CurrentSessionSearch.Add(NewData);
	}
}

// Join Any Available Online Game
void URadeGameInstance::JoinOnlineGame()
{
	ULocalPlayer* const Player = GetFirstGamePlayer();

	// Just a SearchResult where we can save the one we want to use, for the case we find more than one!
	FOnlineSessionSearchResult SearchResult;

	// If the Array is not empty, we can go through it
	if (SessionSearch->SearchResults.Num() > 0) {
		for (int32 i = 0; i < SessionSearch->SearchResults.Num(); i++) {
			// To avoid something crazy, we filter sessions from ourself
			if (SessionSearch->SearchResults[i].Session.OwningUserId != Player->GetPreferredUniqueNetId()) {
				SearchResult = SessionSearch->SearchResults[i];
            JoinSession(Player->GetPreferredUniqueNetId().GetUniqueNetId(), theSessionName, SearchResult);
				break;
			}
		}
	}
}

// Join Selected Online Session
void URadeGameInstance::JoinSelectedOnlineGame(FAvaiableSessionsData SessionData)
{
   ULocalPlayer* const Player = GetFirstGamePlayer();
   JoinSession(Player->GetPreferredUniqueNetId().GetUniqueNetId(), theSessionName, SessionData.SessionData);
}

// Destroy Session 
void URadeGameInstance::DestroySessionAndLeaveGame()
{
	IOnlineSubsystem* OnlineSub = IOnlineSubsystem::Get();
	if (OnlineSub) {
		IOnlineSessionPtr Sessions = OnlineSub->GetSessionInterface();

		if (Sessions.IsValid()) {
			Sessions->AddOnDestroySessionCompleteDelegate_Handle(OnDestroySessionCompleteDelegate);
			Sessions->DestroySession(theSessionName);
		}
	}
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

///													Internal Code
/*
Code Part for CREATING a Session and starting it, as well as handling
the delegates that get called when the creation or start request is done.
*/


/**
*	Function to host a game!
*
*	@Param		UserID			User that started the request
*	@Param		SessionName		Name of the Session
*	@Param		bIsLAN			Is this is LAN Game?
*	@Param		bIsPresence		"Is the Session to create a presence Session"
*	@Param		MaxNumPlayers	Number of Maximum allowed players on this "Session" (Server)
*/
bool URadeGameInstance::HostSession(TSharedPtr<const FUniqueNetId> UserId, FName SessionName, FString MapName, bool bIsLAN, bool bIsPresence, int32 MaxNumPlayers)
{
	// Get the Online Subsystem to work with
	IOnlineSubsystem* const OnlineSub = IOnlineSubsystem::Get();

	if (OnlineSub) {
		// Get the Session Interface, so we can call the "CreateSession" function on it
		IOnlineSessionPtr Sessions = OnlineSub->GetSessionInterface();

		if (Sessions.IsValid() && UserId.IsValid()) {
			/*
			Fill in all the Session Settings that we want to use.

			There are more with SessionSettings.Set(...);
			For example the Map or the GameMode/Type.
			*/

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

			SessionSettings->Set(SETTING_MAPNAME, MapName, EOnlineDataAdvertisementType::ViaOnlineService);

			// Set the delegate to the Handle of the SessionInterface
			OnCreateSessionCompleteDelegateHandle = Sessions->AddOnCreateSessionCompleteDelegate_Handle(OnCreateSessionCompleteDelegate);

			//printr(SessionName.ToString());

			// Our delegate should get called when this is complete (doesn't need to be successful!)
			return Sessions->CreateSession(*UserId, SessionName, *SessionSettings);
			
		}
	} else {
		//GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::Red, TEXT("No OnlineSubsytem found!"));
	}

	return false;
}

/**
*	Delegate fired when a session create request has completed
*
*	@param SessionName the name of the session this callback is for
*	@param bWasSuccessful true if the async action completed without error, false if there was an error
*/
void URadeGameInstance::OnCreateSessionComplete(FName SessionName, bool bWasSuccessful)
{
	//GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::Red, FString::Printf(TEXT("OnCreateSessionComplete %s, %d"), *SessionName.ToString(), bWasSuccessful));

	// Get the OnlineSubsystem so we can get the Session Interface
	IOnlineSubsystem* OnlineSub = IOnlineSubsystem::Get();
	if (OnlineSub) {
		// Get the Session Interface to call the StartSession function
		IOnlineSessionPtr Sessions = OnlineSub->GetSessionInterface();

		if (Sessions.IsValid()) {
			// Clear the SessionComplete delegate handle, since we finished this call
			Sessions->ClearOnCreateSessionCompleteDelegate_Handle(OnCreateSessionCompleteDelegateHandle);
			if (bWasSuccessful) {
				// Set the StartSession delegate handle
				OnStartSessionCompleteDelegateHandle = Sessions->AddOnStartSessionCompleteDelegate_Handle(OnStartSessionCompleteDelegate);

				//printr(SessionName.ToString());
				// Our StartSessionComplete delegate should get called after this
				Sessions->StartSession(SessionName);
			}
		}
	}
}

/**
*	Delegate fired when a session start request has completed
*
*	@param SessionName the name of the session this callback is for
*	@param bWasSuccessful true if the async action completed without error, false if there was an error
*/
void URadeGameInstance::OnStartOnlineGameComplete(FName SessionName, bool bWasSuccessful)
{
	//GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::Red, FString::Printf(TEXT("OnStartSessionComplete %s, %d"), *SessionName.ToString(), bWasSuccessful));

	// Get the Online Subsystem so we can get the Session Interface
	IOnlineSubsystem* OnlineSub = IOnlineSubsystem::Get();
	if (OnlineSub) {
		// Get the Session Interface to clear the Delegate
		IOnlineSessionPtr Sessions = OnlineSub->GetSessionInterface();
		if (Sessions.IsValid()) {
			// Clear the delegate, since we are done with this call
			Sessions->ClearOnStartSessionCompleteDelegate_Handle(OnStartSessionCompleteDelegateHandle);
		}
	}

	// If the start was successful, we can open a NewMap if we want. Make sure to use "listen" as a parameter!
	if (bWasSuccessful) {
		
		//BattleArena
		UGameplayStatics::OpenLevel(GetWorld(), *TheMapName, true, "listen");
	}
}


/**
*	Find an online session
*
*	@param UserId user that initiated the request
*	@param SessionName name of session this search will generate
*	@param bIsLAN are we searching LAN matches
*	@param bIsPresence are we searching presence sessions
*/
void URadeGameInstance::FindSessions(TSharedPtr<const FUniqueNetId> UserId, FName SessionName, bool bIsLAN, bool bIsPresence)
{
	// Get the OnlineSubsystem we want to work with
	IOnlineSubsystem* OnlineSub = IOnlineSubsystem::Get();

	if (OnlineSub) {
		bIsSearchingSession = true;
		// Get the SessionInterface from our OnlineSubsystem
		IOnlineSessionPtr Sessions = OnlineSub->GetSessionInterface();

		if (Sessions.IsValid() && UserId.IsValid()) {
			/*
			Fill in all the SearchSettings, like if we are searching for a LAN game and how many results we want to have!
			*/
			SessionSearch = MakeShareable(new FOnlineSessionSearch());

			SessionSearch->bIsLanQuery = bIsLAN;
			SessionSearch->MaxSearchResults = 20;
			SessionSearch->PingBucketSize = 50;

			// We only want to set this Query Setting if "bIsPresence" is true
			if (bIsPresence) {
				SessionSearch->QuerySettings.Set(SEARCH_PRESENCE, bIsPresence, EOnlineComparisonOp::Equals);
			}

			TSharedRef<FOnlineSessionSearch> SearchSettingsRef = SessionSearch.ToSharedRef();

			// Set the Delegate to the Delegate Handle of the FindSession function
			OnFindSessionsCompleteDelegateHandle = Sessions->AddOnFindSessionsCompleteDelegate_Handle(OnFindSessionsCompleteDelegate);

			// Finally call the SessionInterface function. The Delegate gets called once this is finished
			Sessions->FindSessions(*UserId, SearchSettingsRef);
		}
	} else {
		// If something goes wrong, just call the Delegate Function directly with "false".
		OnFindSessionsComplete(false);
	}
}

/**
*	Delegate fired when a session search query has completed
*
*	@param bWasSuccessful true if the async action completed without error, false if there was an error
*/
void URadeGameInstance::OnFindSessionsComplete(bool bWasSuccessful)
{
	// Start Session Search
	bIsSearchingSession = false;

	// Get OnlineSubsystem we want to work with
	IOnlineSubsystem* const OnlineSub = IOnlineSubsystem::Get();
	if (OnlineSub) {
		// Get SessionInterface of the OnlineSubsystem
		IOnlineSessionPtr Sessions = OnlineSub->GetSessionInterface();
		if (Sessions.IsValid()) {
			// Clear the Delegate handle, since we finished this call
			Sessions->ClearOnFindSessionsCompleteDelegate_Handle(OnFindSessionsCompleteDelegateHandle);
		}
	}

	UpdateSessionList();
}


//		Join Session
bool URadeGameInstance::JoinSession(TSharedPtr<const FUniqueNetId> UserId, FName SessionName, const FOnlineSessionSearchResult& SearchResult)
{
	// Return bool
	bool bSuccessful = false;

	// Get OnlineSubsystem we want to work with
	IOnlineSubsystem* OnlineSub = IOnlineSubsystem::Get();

	if (OnlineSub) {
		// Get SessionInterface from the OnlineSubsystem
		IOnlineSessionPtr Sessions = OnlineSub->GetSessionInterface();

		if (Sessions.IsValid() && UserId.IsValid()) {
			// Set the Handle again
			OnJoinSessionCompleteDelegateHandle = Sessions->AddOnJoinSessionCompleteDelegate_Handle(OnJoinSessionCompleteDelegate);

			// Call the "JoinSession" Function with the passed "SearchResult". The "SessionSearch->SearchResults" can be used to get such a
			// "FOnlineSessionSearchResult" and pass it. Pretty straight forward!
			bSuccessful = Sessions->JoinSession(*UserId, SessionName, SearchResult);
		}
	}

	return bSuccessful;
}

void URadeGameInstance::OnJoinSessionComplete(FName SessionName, EOnJoinSessionCompleteResult::Type Result)
{
	// Get the OnlineSubsystem we want to work with
	IOnlineSubsystem* OnlineSub = IOnlineSubsystem::Get();
	if (OnlineSub) {
		// Get SessionInterface from the OnlineSubsystem
		IOnlineSessionPtr Sessions = OnlineSub->GetSessionInterface();

		if (Sessions.IsValid()) {
			// Clear the Delegate again
			Sessions->ClearOnJoinSessionCompleteDelegate_Handle(OnJoinSessionCompleteDelegateHandle);

			// Get the first local PlayerController, so we can call "ClientTravel" to get to the Server Map
			// This is something the Blueprint Node "Join Session" does automatically!
			APlayerController * const PlayerController = GetFirstLocalPlayerController();

			// We need a FString to use ClientTravel and we can let the SessionInterface construct such a
			// String for us by giving him the SessionName and an empty String. We want to do this, because
			// Every OnlineSubsystem uses different TravelURLs
			FString TravelURL;

			if (PlayerController && Sessions->GetResolvedConnectString(SessionName, TravelURL)) {
				// Finally call the ClienTravel. If you want, you could print the TravelURL to see
				// how it really looks like
				PlayerController->ClientTravel(TravelURL, ETravelType::TRAVEL_Absolute);
			}
		}
	}
}

// Destroy Session And return to Start Map
void URadeGameInstance::OnDestroySessionComplete(FName SessionName, bool bWasSuccessful)
{
	// Get the OnlineSubsystem we want to work with
	IOnlineSubsystem* OnlineSub = IOnlineSubsystem::Get();
	if (OnlineSub) {
		// Get the SessionInterface from the OnlineSubsystem
		IOnlineSessionPtr Sessions = OnlineSub->GetSessionInterface();

		if (Sessions.IsValid()) {
			// Clear the Delegate
			Sessions->ClearOnDestroySessionCompleteDelegate_Handle(OnDestroySessionCompleteDelegateHandle);

			// If it was successful, we just load another level (could be a MainMenu!)
			if (bWasSuccessful) {
				UGameplayStatics::OpenLevel(GetWorld(), "SelectMap", true);
			}
		}
	}
}

