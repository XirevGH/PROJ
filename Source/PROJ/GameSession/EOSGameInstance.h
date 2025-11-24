#pragma once

#define SETTING_SESSIONSTATE FName(TEXT("SessionState"))

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
enum class ESessionStates : uint8
{
	Lobby,
	Transition,
	Playing,
	Ended
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
	void CreateSession(const FName& Name, const bool bUsePresence);
	
	FString GetSessionName(const FOnlineSessionSearchResult& SessionSearchResult) const;

	UFUNCTION(BlueprintCallable)
	void SearchForMatch();
	
	UFUNCTION(BlueprintImplementableEvent)
	void OnMatchSessionFound(const FName Name);

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

	UFUNCTION(BlueprintCallable)
	void SetSessionState(ESessionStates SessionState);
	
	/* ----------- Custom settings -------------- */
	UFUNCTION(BlueprintPure)
	const FString& GetSessionNameKey() const { return SessionNameKey; }
	UFUNCTION(BlueprintPure)
	const FString& GetCustomSessionNameKey() const { return CustomSessionNameKey; }
	UFUNCTION(BlueprintPure)
	const FString& GetSelectedGameModeKey() const { return SelectedGameModeKey; }
	UFUNCTION(BlueprintPure)
	const FString& GetIsSearchingForMatchKey() const { return IsSearchingForMatchKey; }
	
	UFUNCTION(BlueprintPure)
	bool GetIsSearchingForMatch() const;
	UFUNCTION(BlueprintPure)
	FString GetSelectedGameMode() const;
	UFUNCTION(BlueprintPure)
	FString GetCustomSessionName() const;
	UFUNCTION(BlueprintPure)
	FString GetSessionName() const;
	
	UFUNCTION(BlueprintCallable)
	void SetIsSearchingForMatch(const bool bIsSearching);
	UFUNCTION(BlueprintCallable)
	void SetSelectedGameMode(const FString& GameMode);
	UFUNCTION(BlueprintCallable)
	void SetCustomSessionName(const FString& CustomSessionName);
	UFUNCTION(BlueprintCallable)
	void SetSessionName(const FString& NewSessionName);
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
	FString IsSearchingForMatchKey;
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

	UFUNCTION()
	void OnFindOpenPublicSessionsCompleted(const bool bSuccess);
	void OnFindMatchSessionsCompleted(bool bSuccess);
	void OnFindSessionByNameCompleted(bool bWasSuccessful);
	void FindCompatibleMatchSessions();
	void OnDestroySessionCompleted(FName Name, bool bWasSuccessful);
	void OnSessionHiddenBeforeDestroy(FName Name, bool bWasSuccessful);
	
	void LoginCompleted(int NumOfPlayers, bool bWasSuccessful, const FUniqueNetId& UniqueId, const FString& Error);
	void CreateSessionCompleted(FName Name, bool bWasSuccessful);
	void OnJoinSessionCompleted(FName Name, EOnJoinSessionCompleteResult::Type Result);
	void LoadLevelAndListen(const TSoftObjectPtr<UWorld>& LevelToLoad);
	
	void UpdateSessionSettings(FOnlineSessionSettings* NewSessionSettings) const;
	FOnlineSessionSettings* GetSessionSettings() const;
	
	void ClearCachedSession();
};