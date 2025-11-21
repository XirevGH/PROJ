#include "EOSGameInstance.h"
#include "OnlineSubsystem.h"
#include "OnlineSubsystemUtils.h"
#include "OnlineSessionSettings.h"
#include "Interfaces/OnlineIdentityInterface.h"
#include "Interfaces/OnlineSessionInterface.h"
#include "Online/OnlineSessionNames.h"

UEOSGameInstance::UEOSGameInstance() :
	SessionNameKey("SessionNameKey"),
	CustomSessionNameKey("CustomSessionName"),
	SelectedGameModeKey("SelectedGameMode"),
	IsSearchingForMatchKey("IsSearchingForMatch"),
	MaxSearchResults(100)
{}

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
}

void UEOSGameInstance::DestroyCurrentSessionAndJoinCachedSession()
{
	IOnlineSessionPtr SessionInterface = Online::GetSessionInterface(GetWorld());
	if (!SessionInterface.IsValid() || !SessionName.IsValid() || SessionName == NAME_None)
	{
		return;
	}
	
	FOnDestroySessionCompleteDelegate Delegate;
	Delegate.BindUObject(this, &ThisClass::OnDestroySessionCompleted);
	
	DestroySessionDelegateHandle = SessionInterface->AddOnDestroySessionCompleteDelegate_Handle(Delegate);
    
	SessionInterface->DestroySession(SessionName);
	// JoinSavedSession called in OnDestroySessionCompleted
}

void UEOSGameInstance::OnDestroySessionCompleted(FName Name, bool bWasSuccessful)
{
	IOnlineSessionPtr SessionInterface = Online::GetSessionInterface(GetWorld());
	if (SessionInterface.IsValid())
	{
		// Clear the delegate handle
		SessionInterface->ClearOnDestroySessionCompleteDelegate_Handle(DestroySessionDelegateHandle);
		DestroySessionDelegateHandle.Reset();
	}
    
	if (bWasSuccessful)
	{
		// Clean up the local name on success
		SessionName = NAME_None;

		JoinSavedSession();
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

bool UEOSGameInstance::GetIsSearchingForMatch() const
{
	if (const FOnlineSessionSettings* SessionSettings = GetSessionSettings())
	{
		bool bIsSearchingForMatch = false;
		SessionSettings->Get(FName(IsSearchingForMatchKey), bIsSearchingForMatch);
		return bIsSearchingForMatch;
	}
	return false;
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

void UEOSGameInstance::SetSessionName(const FString& NewSessionName)
{
	if (FOnlineSessionSettings* SessionSettings = GetSessionSettings())
	{
		SessionSettings->Set(FName(SessionNameKey),
			FOnlineSessionSetting(NewSessionName, EOnlineDataAdvertisementType::ViaOnlineService));
		UpdateSessionSettings(SessionSettings);
	}
}

void UEOSGameInstance::SetIsSearchingForMatch(const bool bIsSearching)
{
	if (FOnlineSessionSettings* SessionSettings = GetSessionSettings())
	{
		SessionSettings->Set(FName(IsSearchingForMatchKey),
			FOnlineSessionSetting(bIsSearching, EOnlineDataAdvertisementType::ViaOnlineService));
		UpdateSessionSettings(SessionSettings);
	}
}

void UEOSGameInstance::SetSelectedGameMode(const FString& GameMode)
{
	if (FOnlineSessionSettings* SessionSettings = GetSessionSettings())
	{
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
		IOnlineIdentityPtr IdentityPtr = Online::GetIdentityInterface(GetWorld());
		if (!IdentityPtr.IsValid()) return;
		TSharedPtr<const FUniqueNetId> UniqueIdPtr = IdentityPtr->GetUniquePlayerId(0);
		if (!UniqueIdPtr.IsValid()) return;
		CreateSession(FName(IdentityPtr->GetPlayerNickname(*UniqueIdPtr)));
	}
	else
	{
		UE_LOG(LogTemp, Display, TEXT("Login failed: %s"), *Error);
	}
}

void UEOSGameInstance::CreateSession(const FName& Name)
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
		SessionSettings.bUsesPresence = true;
		SessionSettings.bAllowJoinInProgress = true;
		SessionSettings.bAllowJoinViaPresence = true;
		SessionSettings.NumPublicConnections = 6;

		// Custom settings
		SessionSettings.Set(FName(SessionNameKey),
			FOnlineSessionSetting(Name.ToString(), EOnlineDataAdvertisementType::ViaOnlineService));
		SessionSettings.Set(FName(CustomSessionNameKey),
			FOnlineSessionSetting(Name.ToString(), EOnlineDataAdvertisementType::ViaOnlineService));
		SessionSettings.Set(FName(IsSearchingForMatchKey),
			FOnlineSessionSetting(false, EOnlineDataAdvertisementType::ViaOnlineService));
		SessionSettings.Set(FName(SelectedGameModeKey),
			FOnlineSessionSetting(TEXT("1v1"), EOnlineDataAdvertisementType::ViaOnlineService));

		SessionSettings.Set(
		FName(SEARCH_KEYWORDS),
		FOnlineSessionSetting(FString("PublicSession"), EOnlineDataAdvertisementType::ViaOnlineService)
	);

		UE_LOG(LogTemp, Display, TEXT("Creating session with name: %s"), *Name.ToString());
		SessionInterface->CreateSession(0, Name, SessionSettings);
	}
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
	if (Index < 0 || Index > OpenPublicSearch->SearchResults.Num())
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
		JoinSavedSession();
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
	UE_LOG(LogTemp, Display, TEXT("Join session completed."));
	if (Result == EOnJoinSessionCompleteResult::Success)
	{
		IOnlineSessionPtr SessionInterface = Online::GetSessionInterface(GetWorld());
		if (SessionInterface.IsValid())
		{
			FString TravelURL;
			SessionInterface->GetResolvedConnectString(Name, TravelURL);
			GetFirstLocalPlayerController(GetWorld())->ClientTravel(TravelURL, TRAVEL_Absolute);
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
		const FName LevelName = FName(*FPackageName::ObjectPathToPackageName(LevelToLoad.ToString()));
		GetWorld()->ServerTravel(LevelName.ToString() + "?listen");
	}
}

void UEOSGameInstance::SearchForMatch()
{
	UE_LOG(LogTemp, Warning, TEXT("IsSearchingForMatch: %hhd"), GetIsSearchingForMatch());
	if (GetIsSearchingForMatch())
	{
		UE_LOG(LogTemp, Warning, TEXT("StartMatchmakingSearch"));
		FindCompatibleMatchSessions();	
	}
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
	MatchSearch->QuerySettings.Set(FName(IsSearchingForMatchKey), true, EOnlineComparisonOp::Equals);
	MatchSearch->QuerySettings.Set(FName(SelectedGameModeKey), GetSelectedGameMode(), EOnlineComparisonOp::Equals);
	MatchSearch->QuerySettings.Set(FName(CustomSessionNameKey), SessionName.ToString(), EOnlineComparisonOp::NotEquals);
	
	MatchSessionsDelegateHandle = SessionInterface->AddOnFindSessionsCompleteDelegate_Handle(
		FOnFindSessionsCompleteDelegate::CreateUObject(
			this, &UEOSGameInstance::OnFindMatchSessionsCompleted));
	
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
	OpenPublicSearch->QuerySettings.Set(FName(IsSearchingForMatchKey), false, EOnlineComparisonOp::Equals);
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
	
	TArray<FBlueprintSessionResult> SessionResults;
	
	for (const FOnlineSessionSearchResult& Result : OpenPublicSearch->SearchResults)
	{
		const int32 OpenPublicConnections = Result.Session.NumOpenPublicConnections;
		const int32 MaxPublicConnections = Result.Session.SessionSettings.NumPublicConnections;
		if (MaxPublicConnections - OpenPublicConnections < MaxPublicConnections / 2)
		{
			FBlueprintSessionResult BlueprintSessionResult;
			BlueprintSessionResult.OnlineResult = Result;
			SessionResults.Add(BlueprintSessionResult);
		}
	}
	// OpenPublicSearch.Reset();
	UE_LOG(LogTemp, Warning, TEXT("Sessions found after filtering: %d"), SessionResults.Num());
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
