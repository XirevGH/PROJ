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
	StartingMatch,
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
	void CreateSession(const FName& Name, const bool bNotTransition, const bool bUseExactName);
	
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
	void JoinLobbyByResult(const FBlueprintSessionResult& Result);

	UFUNCTION(BlueprintPure)
	FBlueprintSessionResult GetCachedSessionToJoin() const;

	UFUNCTION(BlueprintCallable)
	void LeaveToOwnSession();
	
	/* ----------- Custom settings -------------- */
	UFUNCTION(BlueprintPure, Category = "Settings")
	const FString& GetSessionNameKey() const { return SessionNameKey; }
	UFUNCTION(BlueprintPure, Category = "Settings")
	const FString& GetCustomSessionNameKey() const { return CustomSessionNameKey; }
	UFUNCTION(BlueprintPure, Category = "Settings")
	const FString& GetSelectedGameModeKey() const { return SelectedGameModeKey; }
	UFUNCTION(BlueprintPure, Category = "Settings")
	const FString& GetSessionStateKey() const { return SessionStateKey; }
	
	UFUNCTION(BlueprintPure, Category = "Settings")
	FString GetSelectedGameMode() const;
	UFUNCTION(BlueprintPure, Category = "Settings")
	FString GetCustomSessionName() const;
	UFUNCTION(BlueprintPure, Category = "Settings")
	FString GetSessionName() const;
	UFUNCTION(BlueprintPure, Category = "Settings")
	ESessionState GetSessionState() const;
	
	UFUNCTION(BlueprintCallable, Category = "Settings")
	void SetSelectedGameMode(const FString& GameMode);
	UFUNCTION(BlueprintCallable, Category = "Settings")
	void SetCustomSessionName(const FString& CustomSessionName);
	UFUNCTION(BlueprintCallable, Category = "Settings")
	void SetSessionName(const FString& NewSessionName);
	UFUNCTION(BlueprintCallable, Category = "Settings")
	void SetSessionState(const ESessionState NewSessionState);
	/* ------------------------------------------ */
	
	UFUNCTION(BlueprintCallable)
	void SetNumPublicConnections(const int NewAmount);

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TSoftObjectPtr<UWorld> LobbyLevel;

	UFUNCTION(BlueprintCallable, Category = "Migration")
	void Client_ExecuteLeaderMigration(FString TargetSessionName);

	UFUNCTION(BlueprintCallable, Category = "Migration")
	void Client_ExecuteMemberMigration(FString TargetSessionName);

	UFUNCTION(BlueprintPure, Category = "Team")
	bool GetIsTeamLeader() const { return bIsTeamLeader; }

	UFUNCTION(BlueprintCallable, Category = "Settings")
	void ResetLobbySettings(); 
	
protected:
	virtual void Init() override;

	UFUNCTION(BlueprintCallable)
	void DestroyCurrentSessionAndJoinCachedSession();

	UFUNCTION(BlueprintCallable)
	void SetClientIsTransitioning(const bool bTransitioning) { bClientTransitionToOtherSession = bTransitioning; }

private:
	FName SessionName;
	FString SessionNameKey;
	FString CustomSessionNameKey;
	FString SelectedGameModeKey;
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

	bool bIsTeamLeader;
	bool bIsMigratingLeader;
	bool bIsMigratingMember;
	FString MigrationTargetName;
	
	bool bClientTransitionToOtherSession;
	bool bReturningToOwnLobby;
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
	void SetStartMatchSearchVariables(ESessionState NewSessionState, int NewPublicConnections);
};
