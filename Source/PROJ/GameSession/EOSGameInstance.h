#pragma once

#include "CoreMinimal.h"
#include "FCustomBlueprintSessionResult.h"
#include "Engine/GameInstance.h"
#include "EOSGameInstance.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnOpenPublicLobbiesFound,
	const TArray<FBlueprintSessionResult>&, Sessions);

class IOnlineSubsystem;
/**
 * 
 */
UCLASS()
class PROJ_API UEOSGameInstance : public UGameInstance
{
	GENERATED_BODY()

public:
	UEOSGameInstance();
	
	UFUNCTION(BlueprintCallable)
	void Login();
	
	UFUNCTION(BlueprintCallable)
	void CreateSession(const FName& Name);

	// UFUNCTION(BlueprintCallable)
	// void FindSessions();
	
	FString GetSessionName(const FOnlineSessionSearchResult& SessionSearchResult) const;

	UFUNCTION(BlueprintCallable)
	void SearchForMatch();

	// After a compatible match has been found, call destroy session and join session on success.
	UFUNCTION(BlueprintImplementableEvent)
	void OnMatchSessionFound(const FBlueprintSessionResult FoundMatchSession);

	// Find sessions "lobbies" open for joining
	// Should bind custom event in blueprint to OnOpenPublicLobbiesFound to get the array of sessions
	UFUNCTION(BlueprintCallable)
	void FindOpenPublicSessions();

	UPROPERTY(BlueprintAssignable)
	FOnOpenPublicLobbiesFound OnOpenPublicLobbiesFound;
	
	/* ----------- Custom settings -------------- */
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
	
	UFUNCTION(BlueprintCallable)
	void SetIsSearchingForMatch(const bool bIsSearching);
	UFUNCTION(BlueprintCallable)
	void SetSelectedGameMode(const FString& GameMode);
	UFUNCTION(BlueprintCallable)
	void SetCustomSessionName(const FString& CustomSessionName);
	/* ------------------------------------------ */
	
protected:
	virtual void Init() override;

	// Call in blueprint after JoinSession:OnSuccess 
	UFUNCTION(BlueprintCallable)
	void HandleSuccessfulJoin();

private:
	UPROPERTY(EditDefaultsOnly)
	TSoftObjectPtr<UWorld> LobbyLevel;
	
	FName SessionName;
	FString CustomSessionNameKey;
	FString SelectedGameModeKey;
	FString IsSearchingForMatchKey;
	int32 MaxSearchResults;

	TSharedPtr<FOnlineSessionSearch> MatchSearch;
	TSharedPtr<FOnlineSessionSearch> OpenPublicSearch;

	FDelegateHandle OpenPublicSessionsDelegateHandle;
	FDelegateHandle MatchSessionsDelegateHandle;

	UFUNCTION()
	void OnFindOpenPublicSessionsCompleted(const bool bSuccess);
	void OnFindMatchSessionsCompleted(bool bSuccess);
	void FindCompatibleMatchSessions();
	
	void LoginCompleted(int NumOfPlayers, bool bWasSuccessful, const FUniqueNetId& UniqueId, const FString& Error);
	void CreateSessionCompleted(FName SessionName, bool bWasSuccessful);
	// void FindSessionsCompleted(bool bWasSuccessful);
	// void JoinSessionCompleted(FName Name, EOnJoinSessionCompleteResult::Type Result);
	void LoadLevelAndListen(const TSoftObjectPtr<UWorld>& LevelToLoad);
	
	void UpdateSessionSettings(FOnlineSessionSettings* NewSessionSettings) const;
	FOnlineSessionSettings* GetSessionSettings() const;
};
