// Copyright 2015 Vagen Ayrapetyan

#pragma once

#include "Engine/GameInstance.h"
#include "RadeData.h"
#include "RadeGameInstance.generated.h"

/**
 * 
 */
UCLASS()
class RADE_API URadeGameInstance : public UGameInstance
{
	GENERATED_BODY()
	


public:


	// Constuctor

	URadeGameInstance(const FObjectInitializer& ObjectInitializer);

	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	/////					Callable Events and Functions


	UFUNCTION(BlueprintCallable, Category = "Network|Test")
		void StartOnlineGame();
	UFUNCTION(BlueprintCallable, Category = "Network|Test")
		void StartOnlineGameMap(FString MapName=TEXT("BattleArena"),int32 MaxPlayerNumber=16);

	UFUNCTION(BlueprintCallable, Category = "Network|Test")
		void FindOnlineGames();

	UFUNCTION(BlueprintCallable, Category = "Network|Test")
		void UpdateSessionList();
	UFUNCTION(BlueprintCallable, Category = "Network|Test")
		void JoinOnlineGame();

	UFUNCTION(BlueprintCallable, Category = "Network|Test")
		void JoinSelectedOnlineGame(FAvaiableSessionsData SessionData);
	// TEST BP FUNCTION

	UFUNCTION(BlueprintCallable, Category = "Network|Test")
		void DestroySessionAndLeaveGame();


	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	/////					Data about the Sessions and Game Instance



	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "")
		TArray<FAvaiableSessionsData> CurrentSessionSearch;


	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "")
		bool bIsSearchingSession = false;


	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "")
		FString PlayerName="Empty Name";

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "")
		FLinearColor PlayerColor=FLinearColor::White;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "")
		FVector PlayerMaterialSettings = FVector(0);


	UFUNCTION(BlueprintCallable, Category = "System")
		void SetPlayerStats(FString newPlayerName, FLinearColor newPlayerColor, FVector newMaterialSettings);


	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	/////					Settings


	//	Variable Part for FINDING a Session

	TSharedPtr<class FOnlineSessionSearch> SessionSearch;

	// Session Setting
	TSharedPtr<class FOnlineSessionSettings> SessionSettings;



	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	/////					Delegate


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

	//	Variable Part for JOINING a Session
	

	// Delegate after joining a session 
	FOnJoinSessionCompleteDelegate OnJoinSessionCompleteDelegate;

	// Handle to registered delegate for joining a session 
	FDelegateHandle OnJoinSessionCompleteDelegateHandle;

	//Variable Part for DESTROYING a Session


	// Delegate for destroying a session
	FOnDestroySessionCompleteDelegate OnDestroySessionCompleteDelegate;

	// Handle to registered delegate for destroying a session 
	FDelegateHandle OnDestroySessionCompleteDelegateHandle;




	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	/////								Ingternal Events


	//			Host A Game
	/**
	*	Function to host a game!
	*
	*	@Param		UserID			User that started the request
	*	@Param		SessionName		Name of the Session
	*	@Param		bIsLAN			Is this is LAN Game?
	*	@Param		bIsPresence		"Is the Session to create a presence Session"
	*	@Param		MaxNumPlayers
	*/
	bool HostSession(TSharedPtr<const FUniqueNetId> UserId, FName SessionName, FString MapName, bool bIsLAN, bool bIsPresence, int32 MaxNumPlayers);


	//			Create Session Complete
	/**
	*	Delegate fired when a session create request has completed
	*
	*	@param SessionName the name of the session this callback is for
	*	@param bWasSuccessful true if the async action completed without error, false if there was an error
	*/
	virtual void OnCreateSessionComplete(FName SessionName, bool bWasSuccessful);


	//			Start Online Game Complete
	/**
	*	Delegate fired when a session start request has completed
	*
	*	@param SessionName the name of the session this callback is for
	*	@param bWasSuccessful true if the async action completed without error, false if there was an error
	*/
	void OnStartOnlineGameComplete(FName SessionName, bool bWasSuccessful);

	

	//			Find Sessions
	/**
	*	Find an online session
	*
	*	@param UserId user that initiated the request
	*	@param SessionName name of session this search will generate
	*	@param bIsLAN are we searching LAN matches
	*	@param bIsPresence are we searching presence sessions
	*/
	void FindSessions(TSharedPtr<const FUniqueNetId> UserId, FName SessionName, bool bIsLAN, bool bIsPresence);


	//				Find Session Complete
	/**
	*	Delegate fired when a session search query has completed
	*
	*	@param bWasSuccessful true if the async action completed without error, false if there was an error
	*/
	void OnFindSessionsComplete(bool bWasSuccessful);

	


	//				Join Session
	/**
	*	Joins a session via a search result
	*
	*	@param SessionName name of session
	*	@param SearchResult Session to join
	*
	*	@return bool true if successful, false otherwise
	*/
	bool JoinSession(TSharedPtr<const FUniqueNetId> UserId, FName SessionName, const FOnlineSessionSearchResult& SearchResult);
	// Find an Alternative to this shit// extra
	virtual bool JoinSession(ULocalPlayer* LocalPlayer, const FOnlineSessionSearchResult& SearchResult) { return false; }
	virtual bool JoinSession(ULocalPlayer* LocalPlayer, int32 SessionIndexInSearchResults) { return false; }



	////////				Fired When Join Session is Complete
	/**
	*	Delegate fired when a session join request has completed
	*
	*	@param SessionName the name of the session this callback is for
	*	@param bWasSuccessful true if the async action completed without error, false if there was an error
	*/
	void OnJoinSessionComplete(FName SessionName, EOnJoinSessionCompleteResult::Type Result);



	//					Function part for DESTROYING a Session

	/**
	*	Delegate fired when a destroying an online session has completed
	*
	*	@param SessionName the name of the session this callback is for
	*	@param bWasSuccessful true if the async action completed without error, false if there was an error
	*/
	virtual void OnDestroySessionComplete(FName SessionName, bool bWasSuccessful);


	
	
};
