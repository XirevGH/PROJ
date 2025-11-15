#include "AdvancedGameInstance.h"

UAdvancedGameInstance::UAdvancedGameInstance(const FObjectInitializer& ObjectInitializer) :
	Super(ObjectInitializer),
	IsSearchingForMatchKey("IsSearchingForMatch"),
	SelectedGameModeKey("SelectedGameMode"),
	CustomSessionNameKey("CustomSessionName"),
	UniqueTeamID("None"),
	bIsTeamLeader(false),
	MaxSearchResults(100)
{
	bAutoTravelOnAcceptedUserInviteReceived = false;
}

void UAdvancedGameInstance::SetIsSearchingForMatch(const bool bIsSearching)
{
	if (FOnlineSessionSettings* SessionSettings = GetSessionSettings())
	{
		SessionSettings->Set(FName(IsSearchingForMatchKey),
			FOnlineSessionSetting(bIsSearching, EOnlineDataAdvertisementType::ViaOnlineService));
		UpdateSessionSettings(SessionSettings);
	}
}

void UAdvancedGameInstance::SetSelectedGameMode(const FString& GameMode)
{
	if (FOnlineSessionSettings* SessionSettings = GetSessionSettings())
	{
		SessionSettings->Set(FName(SelectedGameModeKey),
			FOnlineSessionSetting(GameMode, EOnlineDataAdvertisementType::ViaOnlineService));
		UpdateSessionSettings(SessionSettings);
	}
}

void UAdvancedGameInstance::SetCustomSessionName(const FString& SessionName)
{
	if (FOnlineSessionSettings* SessionSettings = GetSessionSettings())
	{
		SessionSettings->Set(FName(CustomSessionNameKey),
			FOnlineSessionSetting(SessionName, EOnlineDataAdvertisementType::ViaOnlineService));
		UpdateSessionSettings(SessionSettings);
	}
}

bool UAdvancedGameInstance::GetIsSearchingForMatch() const
{
	if (FOnlineSessionSettings* SessionSettings = GetSessionSettings())
	{
		bool bIsSearching = false;
		SessionSettings->Get(FName(IsSearchingForMatchKey), bIsSearching);
		return bIsSearching;
	}
	return false;
}

FString UAdvancedGameInstance::GetSelectedGameMode() const
{
	if (FOnlineSessionSettings* SessionSettings = GetSessionSettings())
	{
		FString SelectedGameMode;
		SessionSettings->Get(FName(SelectedGameModeKey), SelectedGameMode);
		return SelectedGameMode;
	}
	return FString();
}

FString UAdvancedGameInstance::GetCustomSessionName() const
{
	if (FOnlineSessionSettings* SessionSettings = GetSessionSettings())
	{
		FString SessionName;
		SessionSettings->Get(FName(CustomSessionNameKey), SessionName);
		return SessionName;
	}
	return FString();
}

void UAdvancedGameInstance::StartMatchmakingSearch()
{
	if (GetIsSearchingForMatch())
	{
		FindCompatibleMatchSessions();	
	}
}

void UAdvancedGameInstance::FindCompatibleMatchSessions()
{
	IOnlineSessionPtr SessionInterface = Online::GetSessionInterface(GetWorld());
	if (!SessionInterface.IsValid()) return;

	MatchSearch = MakeShareable(new FOnlineSessionSearch());
	MatchSearch->MaxSearchResults = MaxSearchResults;

	MatchSearch->QuerySettings.Set(FName(IsSearchingForMatchKey), true, EOnlineComparisonOp::Equals);
	MatchSearch->QuerySettings.Set(FName(SelectedGameModeKey), GetSelectedGameMode(), EOnlineComparisonOp::Equals);
	
	SessionInterface->AddOnFindSessionsCompleteDelegate_Handle(
		FOnFindSessionsCompleteDelegate::CreateUObject(
			this, &ThisClass::OnFindMatchSessionsCompleted));
	SessionInterface->FindSessions(0, MatchSearch.ToSharedRef());
}

void UAdvancedGameInstance::OnFindMatchSessionsCompleted(bool bSuccess)
{
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
		// sessions active and not in lobby
		UE_LOG(LogTemp, Warning, TEXT("No matches found"))
		return;
	}
	
	const int32 RandomIndex = FMath::RandRange(0, MatchSearch->SearchResults.Num() - 1);
	FBlueprintSessionResult BlueprintSessionResult;
	BlueprintSessionResult.OnlineResult = MatchSearch->SearchResults[RandomIndex];
	
	OnMatchSessionFound(BlueprintSessionResult);
}

FOnlineSessionSettings* UAdvancedGameInstance::GetSessionSettings() const
{
	IOnlineSessionPtr SessionInterface = Online::GetSessionInterface(GetWorld());
	if (!SessionInterface.IsValid()) return nullptr;
	return SessionInterface->GetSessionSettings(NAME_GameSession);
}

void UAdvancedGameInstance::UpdateSessionSettings(FOnlineSessionSettings* NewSessionSettings) const
{
	IOnlineSessionPtr SessionInterface = Online::GetSessionInterface(GetWorld());
	if (!SessionInterface.IsValid() || NewSessionSettings == nullptr) return;
	SessionInterface->UpdateSession(NAME_GameSession, *NewSessionSettings, true);
}
