#include "EOSGameInstance.h"
#include "OnlineSubsystem.h"
#include "OnlineSubsystemUtils.h"
#include "OnlineSessionSettings.h"
#include "GameFramework/GameStateBase.h"
#include "Interfaces/OnlineIdentityInterface.h"
#include "Interfaces/OnlineSessionInterface.h"
#include "Kismet/GameplayStatics.h"
#include "Online/OnlineSessionNames.h"

UEOSGameInstance::UEOSGameInstance() :
	SessionNameKey("SessionNameKey"),
	CustomSessionNameKey("CustomSessionName"),
	SelectedGameModeKey("SelectedGameMode"),
	MaxSearchResults(100),
	bClientTransitionToOtherSession(false),
	bReturningToOwnLobby(false)
{
	CurrentSessionState = ESessionState::Lobby;
}

void UEOSGameInstance::Init()
{
	Super::Init();
	
	IOnlineIdentityPtr IdentityPtr = Online::GetIdentityInterface(GetWorld());
	if (IdentityPtr.IsValid())
	{
		IdentityPtr->OnLoginCompleteDelegates->AddUObject(this, &ThisClass::LoginCompleted);
	}

	IOnlineSessionPtr SessionInterface = Online::GetSessionInterface(GetWorld());
	if (SessionInterface.IsValid())
	{
		SessionInterface->OnCreateSessionCompleteDelegates.AddUObject(this, &ThisClass::CreateSessionCompleted);
		SessionInterface->OnJoinSessionCompleteDelegates.AddUObject(this, &ThisClass::OnJoinSessionCompleted);
	}

	if (GEngine)
	{
		GEngine->OnTravelFailure().AddUObject(this, &UEOSGameInstance::HandleTravelFailure);
		GEngine->OnNetworkFailure().AddUObject(this, &UEOSGameInstance::HandleNetworkFailure);
	}

	UE_LOG(LogTemp, Display, TEXT("SessionState lobby int: %hd"), static_cast<int32>(ESessionState::Lobby));
	UE_LOG(LogTemp, Display, TEXT("SessionState SearchingForMatch int: %hd"), static_cast<int32>(ESessionState::SearchingForMatch));
	UE_LOG(LogTemp, Display, TEXT("SessionState JoiningMatch int: %hd"), static_cast<int32>(ESessionState::JoiningMatch));
	UE_LOG(LogTemp, Display, TEXT("SessionState InMatch int: %hd"), static_cast<int32>(ESessionState::InMatch));
	UE_LOG(LogTemp, Display, TEXT("SessionState Transition int: %hd"), static_cast<int32>(ESessionState::Transition));
}

void UEOSGameInstance::DestroyCurrentSessionAndJoinCachedSession()
{
	IOnlineSessionPtr SessionInterface = Online::GetSessionInterface(GetWorld());
	if (!SessionInterface.IsValid()) return;
    
	// Safety check: If we don't have a session, just join immediately
	if (SessionName == NAME_None)
	{
		JoinSavedSession();
		return;
	}
	if (DestroySessionDelegateHandle.IsValid())
	{
		SessionInterface->ClearOnDestroySessionCompleteDelegate_Handle(DestroySessionDelegateHandle);
	}
	
	DestroySessionDelegateHandle = SessionInterface->AddOnDestroySessionCompleteDelegate_Handle(
		FOnDestroySessionCompleteDelegate::CreateUObject(this, &ThisClass::OnDestroySessionCompleted));

	UE_LOG(LogTemp, Display, TEXT("Destroying session: %s"), *SessionName.ToString());
    
	// Direct destroy. This works for both Host (shuts down lobby) and Client (leaves lobby).
	if (!SessionInterface->DestroySession(SessionName))
	{
		// If DestroySession returns false immediately (e.g. session didn't exist), 
		// force the next step so the player isn't stuck.
		UE_LOG(LogTemp, Warning, TEXT("DestroySession failed immediately. Proceeding to join."));
		SessionInterface->ClearOnDestroySessionCompleteDelegate_Handle(DestroySessionDelegateHandle);
		JoinSavedSession();
	}
}

void UEOSGameInstance::OnDestroySessionCompleted(FName Name, bool bWasSuccessful)
{
	UE_LOG(LogTemp, Warning, TEXT("OnDestroySessionCompleted called"));
	IOnlineSessionPtr SessionInterface = Online::GetSessionInterface(GetWorld());
	if (!SessionInterface.IsValid()) return;

	if (DestroySessionDelegateHandle.IsValid())
	{
		SessionInterface->ClearOnDestroySessionCompleteDelegate_Handle(DestroySessionDelegateHandle);
		DestroySessionDelegateHandle.Reset();
	}
    
	if (bWasSuccessful)
	{
		UE_LOG(LogTemp, Warning, TEXT("OnDestroySessionCompleted: Session destroyed with name: %s"), *Name.ToString());
		// Clean up the local name on success
		SessionName = NAME_None;

		if (bReturningToOwnLobby)
		{
			UE_LOG(LogTemp, Display, TEXT("Session destroyed. Now creating own lobby session."));
			bReturningToOwnLobby = false;
			CreateOwnSession();
			return;
		}

		if (bClientTransitionToOtherSession)
		{
			UE_LOG(LogTemp, Warning, TEXT("OnDestroySessionCompleted: Client creating own session before joining match"));
			// Give session a name with random number
			FString NewName = FString::Printf(TEXT("TransitionSession_%d"), FMath::Rand());
			CreateSession(FName(*NewName), false);
			return;
		}

		// FIX: Use SetTimerForNextTick instead of a delay or direct call.
		// This allows the stack to unwind and the NetDriver/EOS SDK to finish cleanup.
		if (UWorld* World = GetWorld())
		{
			World->GetTimerManager().SetTimerForNextTick(this, &UEOSGameInstance::JoinSavedSession);
		}
	}
}

FString UEOSGameInstance::GetSessionName(const FOnlineSessionSearchResult& SessionSearchResult) const
{
	FString OutValue = TEXT("Name None");
	if (!SessionSearchResult.IsValid())
	{
		UE_LOG(LogTemp, Display, TEXT("GetSessionName: SessionSearchResult is invalid"));
		return OutValue;
	}
	SessionSearchResult.Session.SessionSettings.Get(FName(SessionNameKey), OutValue);
	return OutValue;
}

FString UEOSGameInstance::GetSelectedGameMode() const
{
	if (const FOnlineSessionSettings* SessionSettings = GetSessionSettings())
	{
		FString SelectedGameMode;
		SessionSettings->Get(FName(SelectedGameModeKey), SelectedGameMode);
		return SelectedGameMode;
	}
	return FString();
}

FString UEOSGameInstance::GetCustomSessionName() const
{
	if (const FOnlineSessionSettings* SessionSettings = GetSessionSettings())
	{
		FString CustomSessionName;
		SessionSettings->Get(FName(CustomSessionNameKey), CustomSessionName);
		return CustomSessionName;
	}
	return FString();
}

FString UEOSGameInstance::GetSessionName() const
{
	if (const FOnlineSessionSettings* SessionSettings = GetSessionSettings())
	{
		FString Name;
		SessionSettings->Get(FName(SessionNameKey), Name);
		return Name;
	}
	return FString();
}

ESessionState UEOSGameInstance::GetSessionState() const
{
	return CurrentSessionState;
}

void UEOSGameInstance::SetSessionState(const ESessionState NewSessionState)
{
	CurrentSessionState = NewSessionState;
	
	if (FOnlineSessionSettings* SessionSettings = GetSessionSettings())
	{
		SessionSettings->Set(FName(SessionStateKey),
			FOnlineSessionSetting(static_cast<int32>(NewSessionState), EOnlineDataAdvertisementType::ViaOnlineService));
		UpdateSessionSettings(SessionSettings);
		UE_LOG(LogTemp, Display, TEXT("SessionState set to: %hhd"), NewSessionState);
	}
}

void UEOSGameInstance::SetSessionName(const FString& NewSessionName)
{
	if (FOnlineSessionSettings* SessionSettings = GetSessionSettings())
	{
		SessionSettings->Set(FName(SessionNameKey),
			FOnlineSessionSetting(NewSessionName, EOnlineDataAdvertisementType::ViaOnlineService));
		UpdateSessionSettings(SessionSettings);
	}
}

void UEOSGameInstance::SetSelectedGameMode(const FString& GameMode)
{
	if (FOnlineSessionSettings* SessionSettings = GetSessionSettings())
	{
		UE_LOG(LogTemp, Display, TEXT("SetSelectedGameMode: %s"), *GameMode);
		SessionSettings->Set(FName(SelectedGameModeKey),
			FOnlineSessionSetting(GameMode, EOnlineDataAdvertisementType::ViaOnlineService));
		UpdateSessionSettings(SessionSettings);
	}
}

void UEOSGameInstance::SetCustomSessionName(const FString& CustomSessionName)
{
	if (FOnlineSessionSettings* SessionSettings = GetSessionSettings())
	{
		SessionSettings->Set(FName(CustomSessionNameKey),
			FOnlineSessionSetting(CustomSessionName, EOnlineDataAdvertisementType::ViaOnlineService));
		UpdateSessionSettings(SessionSettings);
	}
}

void UEOSGameInstance::Login()
{
	IOnlineIdentityPtr IdentityPtr = Online::GetIdentityInterface(GetWorld());
	if (IdentityPtr.IsValid())
	{
		FOnlineAccountCredentials OnlineAccountCredentials;
		OnlineAccountCredentials.Type = "accountportal";
		OnlineAccountCredentials.Id = "";
		OnlineAccountCredentials.Token = "";
		IdentityPtr->Login(0, OnlineAccountCredentials);
	}
}

void UEOSGameInstance::LoginCompleted(int numOfPlayers, bool bWasSuccessful, const FUniqueNetId& UniqueId, const FString& Error)
{
	if (bWasSuccessful)
	{
		UE_LOG(LogTemp, Display, TEXT("Login successfully completed."));
		CreateOwnSession();
	}
	else
	{
		UE_LOG(LogTemp, Display, TEXT("Login failed: %s"), *Error);
		
		if (IsPlayerLoggedIn())
		{
			CreateOwnSession();
		}
	}
}

void UEOSGameInstance::CreateSession(const FName& Name, const bool bIsTransitionSession)
{
	IOnlineSessionPtr SessionInterface = Online::GetSessionInterface(GetWorld());
	if (SessionInterface.IsValid())
	{
		SessionName = Name;
		
		FOnlineSessionSettings SessionSettings;
		SessionSettings.bAllowInvites = true;
		SessionSettings.bIsDedicated = false;
		SessionSettings.bIsLANMatch = false;
		SessionSettings.bShouldAdvertise = true;
		SessionSettings.bUseLobbiesIfAvailable = true;
		SessionSettings.bUsesPresence = bIsTransitionSession;
		SessionSettings.bAllowJoinInProgress = true;
		SessionSettings.bAllowJoinViaPresence = bIsTransitionSession;
		SessionSettings.NumPublicConnections = 6;

		// Custom settings
		SessionSettings.Set(FName(SessionNameKey),
			FOnlineSessionSetting(Name.ToString(), EOnlineDataAdvertisementType::ViaOnlineService));
		SessionSettings.Set(FName(CustomSessionNameKey),
			FOnlineSessionSetting(Name.ToString(), EOnlineDataAdvertisementType::ViaOnlineService));
		SessionSettings.Set(FName(SelectedGameModeKey),
			FOnlineSessionSetting(TEXT("1v1"), EOnlineDataAdvertisementType::ViaOnlineService));
		SessionSettings.Set(
		FName(SEARCH_KEYWORDS),
			FOnlineSessionSetting(FString("PublicSession"), EOnlineDataAdvertisementType::ViaOnlineService));
		
		if (bIsTransitionSession)
		{
			SessionSettings.Set(FName(SessionStateKey),
				FOnlineSessionSetting(static_cast<int32>(ESessionState::Transition), EOnlineDataAdvertisementType::ViaOnlineService));
			CurrentSessionState = ESessionState::Transition;
		}else
		{
			SessionSettings.Set(FName(SessionStateKey),
				FOnlineSessionSetting(static_cast<int32>(ESessionState::Lobby), EOnlineDataAdvertisementType::ViaOnlineService));
			CurrentSessionState = ESessionState::Lobby;
		}

		UE_LOG(LogTemp, Display, TEXT("Creating session with name: %s"), *Name.ToString());
		SessionInterface->CreateSession(0, Name, SessionSettings);
	}
}

void UEOSGameInstance::CreateOwnSession()
{
	IOnlineIdentityPtr IdentityPtr = Online::GetIdentityInterface(GetWorld());
	if (!IdentityPtr.IsValid()) return;
	TSharedPtr<const FUniqueNetId> UniqueIdPtr = IdentityPtr->GetUniquePlayerId(0);
	if (!UniqueIdPtr.IsValid()) return;
	UE_LOG(LogTemp, Display, TEXT("Creating own session"));
	CreateSession(FName(IdentityPtr->GetPlayerNickname(*UniqueIdPtr)), false);
}

void UEOSGameInstance::CreateSessionCompleted(FName Name, bool bWasSuccessful)
{
	if (bWasSuccessful)
	{
		UE_LOG(LogTemp, Display, TEXT("Session created successfully with name: %s"), *Name.ToString());
	}
	else
	{
		UE_LOG(LogTemp, Display, TEXT("Session creation failed"));
	}
	LoadLevelAndListen(LobbyLevel);

	if (bClientTransitionToOtherSession)
	{
		bClientTransitionToOtherSession = false;
		UE_LOG(LogTemp, Display, TEXT("Previous client is now calling to destroy new session and join match"));
		DestroyCurrentSessionAndJoinCachedSession();
	}
}

void UEOSGameInstance::JoinSavedSession()
{
	IOnlineSessionPtr SessionInterface = Online::GetSessionInterface(GetWorld());
	if (!SessionInterface.IsValid()) return;
	if (!CachedSessionToJoin.IsValid())
	{
		UE_LOG(LogTemp, Warning, TEXT("Cached session is invalid"));
		return;
	}
	const FName Name = FName(GetSessionName(CachedSessionToJoin));
	UE_LOG(LogTemp, Warning, TEXT("Trying to join session with name: %s"), *Name.ToString());
	SessionInterface->JoinSession(0, Name, CachedSessionToJoin);
}

void UEOSGameInstance::JoinLobbyByIndex(const int32 Index)
{
	if (!OpenPublicSearch.IsValid())
	{
		UE_LOG(LogTemp, Warning, TEXT("OpenPublicSearch is invalid"));
		return;
	}
	if (Index < 0 || Index >= OpenPublicSearch->SearchResults.Num())
	{
		UE_LOG(LogTemp, Error, TEXT("JoinLobbyByIndex: Index is out of range"));
		return;
	}
	IOnlineSessionPtr SessionInterface = Online::GetSessionInterface(GetWorld());
	if (SessionInterface.IsValid())
	{
		UE_LOG(LogTemp, Display, TEXT("Trying to join session with index %d"), Index);
		UE_LOG(LogTemp, Display, TEXT("Session by index name is: %s"), *GetSessionName(OpenPublicSearch->SearchResults[Index]));
		CachedSessionToJoin = OpenPublicSearch->SearchResults[Index];
		
		if (UWorld* World = GetWorld())
		{
			if (World->GetNetMode() == NM_Client)
			{
				bClientTransitionToOtherSession = true;
			}
		}
		
		DestroyCurrentSessionAndJoinCachedSession();
	}
}

FBlueprintSessionResult UEOSGameInstance::GetCachedSessionToJoin() const
{
	if (!CachedSessionToJoin.IsValid())
	{
		UE_LOG(LogTemp, Error, TEXT("GetCachedSessionToJoin is invalid"));
		return FBlueprintSessionResult();
	}
	FBlueprintSessionResult Result;
	Result.OnlineResult = CachedSessionToJoin;
	return Result;
}

void UEOSGameInstance::OnJoinSessionCompleted(FName Name, EOnJoinSessionCompleteResult::Type Result)
{
	UE_LOG(LogTemp, Display, TEXT("Join session completed called"));
	if (Result == EOnJoinSessionCompleteResult::Success)
	{
		UE_LOG(LogTemp, Display, TEXT("Successfully completed JoinSession with name: %s"), *Name.ToString());

		IOnlineSessionPtr SessionInterface = Online::GetSessionInterface(GetWorld());
		if (SessionInterface.IsValid())
		{
			FString TravelURL;
			bool bResolvedConnection = SessionInterface->GetResolvedConnectString(Name, TravelURL);
			if (bResolvedConnection)
			{
				UE_LOG(LogTemp, Display, TEXT("Resolved connection to URL: %s, and Name: %s"), *TravelURL, *Name.ToString());
			} else
			{
				UE_LOG(LogTemp, Display, TEXT("Failed to resolve connection, URL: %s, and Name: %s"), *TravelURL, *Name.ToString());
			}
			if (TravelURL.IsEmpty())
			{
				UE_LOG(LogTemp, Display, TEXT("Travel URL is empty"));
			}
			
			UE_LOG(LogTemp, Display, TEXT("Initiate client travel"));
			GetFirstLocalPlayerController(GetWorld())->ClientTravel(TravelURL, TRAVEL_Absolute);
		}
		else
		{
			UE_LOG(LogTemp, Error, TEXT("OnJoinSessionCompleted: SessionInterface is invalid"));
		}
		SessionName = Name;
		ClearCachedSession();
	}
}

void UEOSGameInstance::LoadLevelAndListen(const TSoftObjectPtr<UWorld>& LevelToLoad)
{
	if (!LevelToLoad.IsValid())
	{
		LevelToLoad.LoadSynchronous();
	}
	if (LevelToLoad.IsValid())
	{
		UE_LOG(LogTemp, Display, TEXT("Initiate ServerTravel to lobby after creating session"));
		const FName LevelName = FName(*FPackageName::ObjectPathToPackageName(LevelToLoad.ToString()));
		// GetWorld()->ServerTravel(LevelName.ToString() + "?listen");
		UGameplayStatics::OpenLevel(GetWorld(), LevelName, true, "?listen");
	}
}

void UEOSGameInstance::LeaveToOwnSession()
{
	IOnlineSessionPtr SessionInterface = Online::GetSessionInterface(GetWorld());
	if (!SessionInterface.IsValid()) return;

	UE_LOG(LogTemp, Display, TEXT("Leave to own session"));

	int32 NumPlayers = 0;
	bool bIsHost = false;

	if (const UWorld* World = GetWorld())
	{
		if (const AGameStateBase* GameState = World->GetGameState())
		{
			NumPlayers = GameState->PlayerArray.Num();
		}
		if (World->GetNetMode() != NM_Client)
		{
			bIsHost = true;
		}
	}

	if (bIsHost && NumPlayers <= 1)
	{
		UE_LOG(LogTemp, Warning, TEXT("LeaveToOwnSession: Already host of an empty lobby. Aborting to prevent reload."));
		return;
	}

	bReturningToOwnLobby = true;
	DestroyCurrentSessionAndJoinCachedSession();
}

void UEOSGameInstance::SearchForMatch()
{
	SetSessionState(ESessionState::SearchingForMatch);
	UE_LOG(LogTemp, Warning, TEXT("StartMatchmakingSearch"));
	FindCompatibleMatchSessions();
}

void UEOSGameInstance::FindCompatibleMatchSessions()
{
	UE_LOG(LogTemp, Warning, TEXT("Called FindCompatibleMatchSessions"));
	
	IOnlineSessionPtr SessionInterface = Online::GetSessionInterface(GetWorld());
	if (!SessionInterface.IsValid()) return;
	
	if (MatchSessionsDelegateHandle.IsValid())
	{
		SessionInterface->ClearOnFindSessionsCompleteDelegate_Handle(MatchSessionsDelegateHandle);
	}

	MatchSearch = MakeShareable(new FOnlineSessionSearch());
	MatchSearch->MaxSearchResults = MaxSearchResults;
	MatchSearch->QuerySettings.Set(SEARCH_LOBBIES, true, EOnlineComparisonOp::Equals);
	MatchSearch->QuerySettings.Set(SEARCH_KEYWORDS,FString("PublicSession"),EOnlineComparisonOp::Equals);
	MatchSearch->QuerySettings.Set(FName(SessionStateKey), static_cast<int32>(ESessionState::SearchingForMatch), EOnlineComparisonOp::Equals);
	MatchSearch->QuerySettings.Set(FName(SelectedGameModeKey), GetSelectedGameMode(), EOnlineComparisonOp::Equals);
	MatchSearch->QuerySettings.Set(FName(CustomSessionNameKey), SessionName.ToString(), EOnlineComparisonOp::NotEquals);
	
	MatchSessionsDelegateHandle = SessionInterface->AddOnFindSessionsCompleteDelegate_Handle(
		FOnFindSessionsCompleteDelegate::CreateUObject(
			this, &UEOSGameInstance::OnFindMatchSessionsCompleted));

	UE_LOG(LogTemp, Warning, TEXT("Finding match sessions with game mode: %s"), *GetSelectedGameMode());
	SessionInterface->FindSessions(0, MatchSearch.ToSharedRef());
}

void UEOSGameInstance::OnFindMatchSessionsCompleted(bool bSuccess)
{
	IOnlineSessionPtr SessionInterface = Online::GetSessionInterface(GetWorld());
	if (SessionInterface.IsValid() && MatchSessionsDelegateHandle.IsValid())
	{
		SessionInterface->ClearOnFindSessionsCompleteDelegate_Handle(MatchSessionsDelegateHandle);
		MatchSessionsDelegateHandle.Reset();
	}
	if (!bSuccess)
	{
		UE_LOG(LogTemp, Warning, TEXT("FindSessions failed"))
		return;
	}
	if (!MatchSearch.IsValid())
	{
		UE_LOG(LogTemp, Warning, TEXT("MatchSearch is invalid"))
		return;
	}
	if (MatchSearch->SearchResults.Num() == 0)
	{
		// Do nothing, wait for another session to search for a match and then join this one
		// Implement a clock in the UI to see how long the search have been active, perhaps show number of other
		// sessions active and in lobby
		UE_LOG(LogTemp, Warning, TEXT("No matches found"))
		return;
	}
	
	const int32 RandomIndex = FMath::RandRange(0, MatchSearch->SearchResults.Num() - 1);
	
	CachedSessionToJoin = MatchSearch->SearchResults[RandomIndex];

	MatchSearch.Reset();
	UE_LOG(LogTemp, Warning, TEXT("FindMatchSessionsCompleted"));
	OnMatchSessionFound(FName(GetSessionName(CachedSessionToJoin)));
}

void UEOSGameInstance::FindSessionByName(const FString Name)
{
	UE_LOG(LogTemp, Warning, TEXT("Called FindSessionByName"));

	IOnlineSessionPtr SessionInterface = Online::GetSessionInterface(GetWorld());
	if (!SessionInterface.IsValid()) return;

	if (FindSessionByNameDelegateHandle.IsValid())
	{
		SessionInterface->ClearOnFindSessionsCompleteDelegate_Handle(FindSessionByNameDelegateHandle);
	}

	NameSearch = MakeShareable(new FOnlineSessionSearch());
	NameSearch->MaxSearchResults = 1;
	NameSearch->bIsLanQuery = false;
	NameSearch->QuerySettings.Set(SEARCH_LOBBIES, true, EOnlineComparisonOp::Equals);
	NameSearch->QuerySettings.Set(SEARCH_KEYWORDS,FString("PublicSession"),EOnlineComparisonOp::Equals);
	NameSearch->QuerySettings.Set(FName(SessionNameKey), Name, EOnlineComparisonOp::Equals);

	FindSessionByNameDelegateHandle = SessionInterface->AddOnFindSessionsCompleteDelegate_Handle(
		FOnFindSessionsCompleteDelegate::CreateUObject(this, &UEOSGameInstance::OnFindSessionByNameCompleted));

	UE_LOG(LogTemp, Log, TEXT("Searching for session with name: %s"), *Name);
	SessionInterface->FindSessions(0, NameSearch.ToSharedRef());
}

void UEOSGameInstance::OnFindSessionByNameCompleted(bool bWasSuccessful)
{
	IOnlineSessionPtr SessionInterface = Online::GetSessionInterface(GetWorld());
	if (SessionInterface.IsValid())
	{
		SessionInterface->ClearOnFindSessionsCompleteDelegate_Handle(FindSessionByNameDelegateHandle);
	}

	FBlueprintSessionResult FoundResult;
	
	if (bWasSuccessful && NameSearch.IsValid() && NameSearch->SearchResults.Num() > 0)
	{
		UE_LOG(LogTemp, Log, TEXT("Successfully found session by name."));
		CachedSessionToJoin = NameSearch->SearchResults[0];
		UE_LOG(LogTemp, Warning, TEXT("CachedSessionToJoin updated, current name is: %s"), *GetSessionName(CachedSessionToJoin));
		FoundResult.OnlineResult = NameSearch->SearchResults[0];
		OnSessionFoundByName.Broadcast(true, FoundResult);
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Could not find session by name."));
		OnSessionFoundByName.Broadcast(false, FoundResult);
	}
	NameSearch.Reset();
}

bool UEOSGameInstance::CancelMatchSearch()
{
	UE_LOG(LogTemp, Warning, TEXT("CancelMatchSearch called"));
	if (CurrentSessionState == ESessionState::JoiningMatch)
	{
		UE_LOG(LogTemp, Warning, TEXT("CancelMatchSearch failed due to joining match"));
		return false;
	}
	SetSessionState(ESessionState::Lobby);
	return true;
}

void UEOSGameInstance::FindOpenPublicSessions()
{
	UE_LOG(LogTemp, Warning, TEXT("Called FindOpenPublicSessions"));
	
	IOnlineSessionPtr SessionInterface = Online::GetSessionInterface(GetWorld());
	if (!SessionInterface.IsValid()) return;
	
	if (OpenPublicSessionsDelegateHandle.IsValid())
	{
		SessionInterface->ClearOnFindSessionsCompleteDelegate_Handle(OpenPublicSessionsDelegateHandle);
	}
	
	OpenPublicSearch = MakeShareable(new FOnlineSessionSearch());
	OpenPublicSearch->bIsLanQuery = false;
	OpenPublicSearch->QuerySettings.Set(SEARCH_LOBBIES, true, EOnlineComparisonOp::Equals);
	OpenPublicSearch->QuerySettings.Set(SEARCH_KEYWORDS,FString("PublicSession"),EOnlineComparisonOp::Equals);
	// Only find sessions that are just in the lobby without searching for match or joining a match
	OpenPublicSearch->QuerySettings.Set(FName(SessionStateKey), static_cast<int32>(ESessionState::Lobby), EOnlineComparisonOp::Equals);
	OpenPublicSearch->QuerySettings.Set(FName(CustomSessionNameKey), SessionName.ToString(), EOnlineComparisonOp::NotEquals);
	OpenPublicSearch->MaxSearchResults = MaxSearchResults;
	
	OpenPublicSessionsDelegateHandle = SessionInterface->AddOnFindSessionsCompleteDelegate_Handle(
		FOnFindSessionsCompleteDelegate::CreateUObject(
			this, &UEOSGameInstance::OnFindOpenPublicSessionsCompleted));
	
	SessionInterface->FindSessions(0, OpenPublicSearch.ToSharedRef());
}

void UEOSGameInstance::OnFindOpenPublicSessionsCompleted(const bool bSuccess)
{
	UE_LOG(LogTemp, Warning, TEXT("Sessions found before filtering: %d"), OpenPublicSearch->SearchResults.Num());
	
	IOnlineSessionPtr SessionInterface = Online::GetSessionInterface(GetWorld());
	if (SessionInterface.IsValid() && OpenPublicSessionsDelegateHandle.IsValid())
	{
		SessionInterface->ClearOnFindSessionsCompleteDelegate_Handle(OpenPublicSessionsDelegateHandle);
		OpenPublicSessionsDelegateHandle.Reset();
	}
	
	if (!bSuccess || !OpenPublicSearch.IsValid())
	{
		UE_LOG(LogTemp, Warning, TEXT("FindOpenPublicSessions failed."));
		return;
	}

	UE_LOG(LogTemp, Warning, TEXT("Raw Sessions found: %d"), OpenPublicSearch->SearchResults.Num());

	FilterOpenPublicSearchResults();

	UE_LOG(LogTemp, Display, TEXT("Sessions found after filtering: %d"), OpenPublicSearch->SearchResults.Num());

	// Sort the results before broadcasting
	SortOpenPublicSearchResultsByName();
	
	TArray<FBlueprintSessionResult> SessionResults;
	for (const FOnlineSessionSearchResult& Result : OpenPublicSearch->SearchResults)
	{
		FBlueprintSessionResult BlueprintSessionResult;
		BlueprintSessionResult.OnlineResult = Result;
		SessionResults.Add(BlueprintSessionResult);
		UE_LOG(LogTemp, Display, TEXT("Added session to SessionResults"));
	}

	UE_LOG(LogTemp, Warning, TEXT("Valid/Sorted Sessions broadcasted: %d"), SessionResults.Num());
	UE_LOG(LogTemp, Warning, TEXT("FindOpenPublicSessionsCompleted"));
	OnOpenPublicLobbiesFound.Broadcast(SessionResults);
}

FOnlineSessionSettings* UEOSGameInstance::GetSessionSettings() const
{
	IOnlineSessionPtr SessionInterface = Online::GetSessionInterface(GetWorld());
	if (!SessionInterface.IsValid()) return nullptr;

	FOnlineSessionSettings* SessionSettings = SessionInterface->GetSessionSettings(SessionName);
	if (!SessionSettings)
	{
		UE_LOG(LogTemp, Warning, TEXT("Failed to get session settings from session name: %s"), *SessionName.ToString());
	}
	
	return SessionSettings;
}

void UEOSGameInstance::ClearCachedSession()
{
	CachedSessionToJoin = FOnlineSessionSearchResult(); 
	UE_LOG(LogTemp, Log, TEXT("Cached Session has been cleared."));
}

void UEOSGameInstance::UpdateSessionSettings(FOnlineSessionSettings* NewSessionSettings) const
{
	IOnlineSessionPtr SessionInterface = Online::GetSessionInterface(GetWorld());
	if (!SessionInterface.IsValid() || NewSessionSettings == nullptr) return;
	SessionInterface->UpdateSession(SessionName, *NewSessionSettings, true);
	UE_LOG(LogTemp, Warning, TEXT("Session settings updated"));
}

void UEOSGameInstance::HandleTravelFailure(UWorld* World, ETravelFailure::Type FailureType,
	const FString& ErrorString)
{
	UE_LOG(LogTemp, Display, TEXT("Travel failure: %s"), *ErrorString);
	CreateOwnSession();
}

void UEOSGameInstance::HandleNetworkFailure(UWorld* World, UNetDriver* NetDriver,
	ENetworkFailure::Type FailureType, const FString& ErrorString)
{
	UE_LOG(LogTemp, Display, TEXT("Network failure: %s"), *ErrorString);
}

bool UEOSGameInstance::IsPlayerLoggedIn() const
{
	IOnlineIdentityPtr IdentityInterface = Online::GetIdentityInterface(GetWorld());
    
	if (IdentityInterface.IsValid())
	{
		ELoginStatus::Type Status = IdentityInterface->GetLoginStatus(0);
		
		if (Status == ELoginStatus::LoggedIn)
		{
			if (IdentityInterface->GetUniquePlayerId(0).IsValid())
			{
				return true;
			}
		}
	}
	return false;
}

void UEOSGameInstance::FilterOpenPublicSearchResults()
{
	// Identify "Ghost" Sessions (Sessions owned by me)
	IOnlineIdentityPtr IdentityInterface = Online::GetIdentityInterface(GetWorld());
	FUniqueNetIdRepl MyUniqueId;
	if (IdentityInterface.IsValid())
	{
		MyUniqueId = IdentityInterface->GetUniquePlayerId(0);
	}
	
	TArray<FOnlineSessionSearchResult> ValidSearchResults;

	for (const FOnlineSessionSearchResult& Result : OpenPublicSearch->SearchResults)
	{
		const int32 OpenPublicConnections = Result.Session.NumOpenPublicConnections;
		const int32 MaxPublicConnections = Result.Session.SessionSettings.NumPublicConnections;
		const int32 CurrentPlayers = MaxPublicConnections - OpenPublicConnections;

		// Filter ghost sessions
		if (!Result.Session.OwningUserId.IsValid())
		{
			UE_LOG(LogTemp, Display, TEXT("Session has no valid owner"));
			continue;
		}

		// Filter 2: Ghost Session (CRITICAL)
		// If I own this session, hide it. It's either the one I'm in, or one I just destroyed.
		if (MyUniqueId.IsValid() && *Result.Session.OwningUserId == *MyUniqueId)
		{
			UE_LOG(LogTemp, Display, TEXT("Session is my own session"));
			continue;
		}
		
		if (CurrentPlayers >= MaxPublicConnections / 2)
		{
			UE_LOG(LogTemp, Display, TEXT("CurrentPlayers are more than half the max amount"));
			continue;
		}

		if (CurrentPlayers < 0)
		{
			UE_LOG(LogTemp, Display, TEXT("CurrentPlayers is negative"));
			continue;
		}
		
		ValidSearchResults.Add(Result);
		UE_LOG(LogTemp, Display, TEXT("Session found and added to valid results"));
	}

	OpenPublicSearch->SearchResults = ValidSearchResults;
}

void UEOSGameInstance::SortOpenPublicSearchResultsByName()
{
	Algo::Sort(OpenPublicSearch->SearchResults, [&](const FOnlineSessionSearchResult& A, const FOnlineSessionSearchResult& B)
	{
		FString NameA, NameB;
		A.Session.SessionSettings.Get(FName(SessionNameKey), NameA);
		B.Session.SessionSettings.Get(FName(SessionNameKey), NameB);
		return NameA < NameB; 
	});
}