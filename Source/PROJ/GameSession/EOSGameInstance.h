#pragma once

#include "CoreMinimal.h"
#include "FCustomBlueprintSessionResult.h"
#include "Engine/GameInstance.h"
#include "EOSGameInstance.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnOpenPublicLobbiesFound,
	const TArray<FBlueprintSessionResult>&, Sessions);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnSessionFoundByName,
	bool, bWasSuccessful, const FBlueprintSessionResult&, Session);

class IOnlineSubsystem;

UENUM(BlueprintType)
enum class ESessionState : uint8
{
	Lobby,
	SearchingForMatch,
	JoiningMatch,
	InMatch,
	Transition
};

UCLASS()
class PROJ_API UEOSGameInstance : public UGameInstance
{
	GENERATED_BODY()

public:
	UEOSGameInstance();
	
	UFUNCTION(BlueprintCallable)
	void Login();
	
	UFUNCTION(BlueprintCallable)
	void CreateSession(const FName& Name, const bool bIsTransitionSession);
	
	FString GetSessionName(const FOnlineSessionSearchResult& SessionSearchResult) const;

	UFUNCTION(BlueprintCallable)
	void SearchForMatch();
	
	UFUNCTION(BlueprintImplementableEvent)
	void OnMatchSessionFound(const FName Name);

	UFUNCTION(BlueprintCallable)
	bool CancelMatchSearch();

	// Find sessions "lobbies" open for joining
	// Should bind custom event in blueprint to OnOpenPublicLobbiesFound to get the array of sessions
	UFUNCTION(BlueprintCallable)
	void FindOpenPublicSessions();

	UPROPERTY(BlueprintAssignable)
	FOnOpenPublicLobbiesFound OnOpenPublicLobbiesFound;

	UFUNCTION(BlueprintCallable)
	void FindSessionByName(const FString Name);

	UPROPERTY(BlueprintAssignable)
	FOnSessionFoundByName OnSessionFoundByName;

	UFUNCTION(BlueprintCallable)
	void JoinSavedSession();

	UFUNCTION(BlueprintCallable)
	void JoinLobbyByIndex(const int32 Index);

	UFUNCTION(BlueprintPure)
	FBlueprintSessionResult GetCachedSessionToJoin() const;
	
	/* ----------- Custom settings -------------- */
	UFUNCTION(BlueprintPure)
	const FString& GetSessionNameKey() const { return SessionNameKey; }
	UFUNCTION(BlueprintPure)
	const FString& GetCustomSessionNameKey() const { return CustomSessionNameKey; }
	UFUNCTION(BlueprintPure)
	const FString& GetSelectedGameModeKey() const { return SelectedGameModeKey; }
	// UFUNCTION(BlueprintPure)
	// const FString& GetIsSearchingForMatchKey() const { return IsSearchingForMatchKey; }
	UFUNCTION(BlueprintPure)
	const FString& GetSessionStateKey() const { return SessionStateKey; }
	
	// UFUNCTION(BlueprintPure)
	// bool GetIsSearchingForMatch() const;
	UFUNCTION(BlueprintPure)
	FString GetSelectedGameMode() const;
	UFUNCTION(BlueprintPure)
	FString GetCustomSessionName() const;
	UFUNCTION(BlueprintPure)
	FString GetSessionName() const;
	UFUNCTION(BlueprintPure)
	ESessionState GetSessionState() const;
	
	// UFUNCTION(BlueprintCallable)
	// void SetIsSearchingForMatch(const bool bIsSearching);
	UFUNCTION(BlueprintCallable)
	void SetSelectedGameMode(const FString& GameMode);
	UFUNCTION(BlueprintCallable)
	void SetCustomSessionName(const FString& CustomSessionName);
	UFUNCTION(BlueprintCallable)
	void SetSessionName(const FString& NewSessionName);
	UFUNCTION(BlueprintCallable)
	void SetSessionState(const ESessionState NewSessionState);
	/* ------------------------------------------ */

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TSoftObjectPtr<UWorld> LobbyLevel;
	
protected:
	virtual void Init() override;

	UFUNCTION(BlueprintCallable)
	void DestroyCurrentSessionAndJoinCachedSession();

	UFUNCTION(BlueprintCallable)
	void SetClientIsJoiningMatch(const bool bJoining) { bClientJoiningMatch = bJoining; }

private:
	FName SessionName;
	FString SessionNameKey;
	FString CustomSessionNameKey;
	FString SelectedGameModeKey;
	// FString IsSearchingForMatchKey;
	FString SessionStateKey;
	int32 MaxSearchResults;

	TSharedPtr<FOnlineSessionSearch> MatchSearch;
	TSharedPtr<FOnlineSessionSearch> OpenPublicSearch;
	TSharedPtr<FOnlineSessionSearch> NameSearch;

	FOnlineSessionSearchResult CachedSessionToJoin;

	FDelegateHandle OpenPublicSessionsDelegateHandle;
	FDelegateHandle MatchSessionsDelegateHandle;
	FDelegateHandle FindSessionByNameDelegateHandle;
	FDelegateHandle DestroySessionDelegateHandle;
	FDelegateHandle UpdateSessionDelegateHandle;

	bool bClientJoiningMatch;
	ESessionState CurrentSessionState;

	UFUNCTION()
	void OnFindOpenPublicSessionsCompleted(const bool bSuccess);
	void OnFindMatchSessionsCompleted(bool bSuccess);
	void OnFindSessionByNameCompleted(bool bWasSuccessful);
	void FindCompatibleMatchSessions();
	void OnDestroySessionCompleted(FName Name, bool bWasSuccessful);
	
	void LoginCompleted(int NumOfPlayers, bool bWasSuccessful, const FUniqueNetId& UniqueId, const FString& Error);
	void CreateSessionCompleted(FName Name, bool bWasSuccessful);
	void OnJoinSessionCompleted(FName Name, EOnJoinSessionCompleteResult::Type Result);
	void LoadLevelAndListen(const TSoftObjectPtr<UWorld>& LevelToLoad);
	
	void UpdateSessionSettings(FOnlineSessionSettings* NewSessionSettings) const;
	FOnlineSessionSettings* GetSessionSettings() const;
	
	void ClearCachedSession();

	void HandleTravelFailure(UWorld* World, ETravelFailure::Type FailureType, const FString& ErrorString);
	void HandleNetworkFailure(UWorld* World, UNetDriver* NetDriver, ENetworkFailure::Type FailureType, const FString& ErrorString);

	bool IsPlayerLoggedIn() const;
	void CreateOwnSession();
	void FilterOpenPublicSearchResults();
	void SortOpenPublicSearchResultsByName();
};
