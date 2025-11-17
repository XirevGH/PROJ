#pragma once

#include "CoreMinimal.h"
#include "AdvancedFriendsGameInstance.h"
#include "FCustomBlueprintSessionResult.h"
#include "AdvancedGameInstance.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnOpenPublicSessionsFound, const TArray<FCustomBlueprintSessionResult>&, Sessions);

/**
 * 
 */
UCLASS()
class PROJ_API UAdvancedGameInstance : public UAdvancedFriendsGameInstance
{
	GENERATED_BODY()

public:
	UAdvancedGameInstance(const FObjectInitializer& ObjectInitializer);

	UFUNCTION(BlueprintCallable, Category = "GameSession")
	void SetIsSearchingForMatch(const bool bIsSearching);

	UFUNCTION(BlueprintCallable, Category = "GameSession")
	void SetSelectedGameMode(const FString& GameMode);

	UFUNCTION(BlueprintCallable, Category = "GameSession")
	void SetCustomSessionName(const FString& SessionName);

	UFUNCTION(BlueprintPure, Category = "GameSession")
	bool GetIsSearchingForMatch() const;

	UFUNCTION(BlueprintPure, Category = "GameSession")
	FString GetSelectedGameMode() const;

	UFUNCTION(BlueprintPure, Category = "GameSession")
	FString GetCustomSessionName() const;

	UFUNCTION(BlueprintPure, Category = "GameSession")
	FString GetUniqueTeamID() const { return UniqueTeamID; }

	UFUNCTION(BlueprintPure, Category = "GameSession")
	bool GetIsTeamLeader() const { return bIsTeamLeader; }

	UFUNCTION(BlueprintCallable, Category = "GameSession")
	void StartMatchmakingSearch();

	// After a compatible match has been found, call destroy session and join session on success.
	UFUNCTION(BlueprintImplementableEvent, Category = "GameSession")
	void OnMatchSessionFound(const FBlueprintSessionResult& FoundMatchSession);

	// Find sessions "lobbies" open for joining
	// Should bind custom event in blueprint to OnOpenPublicSessionsFound to get the array of sessions
	UFUNCTION(BlueprintCallable, Category = "GameSession")
	void FindOpenPublicSessions();

	UPROPERTY(BlueprintAssignable, Category = "GameSession")
	FOnOpenPublicSessionsFound OnOpenPublicSessionsFound;

protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "GameSession")
	FString IsSearchingForMatchKey;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "GameSession")
	FString SelectedGameModeKey;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "GameSession")
	FString CustomSessionNameKey;

	UPROPERTY(BlueprintReadWrite, Category = "GameSession")
	FString UniqueTeamID;

	UPROPERTY(BlueprintReadWrite, Category = "GameSession")
	bool bIsTeamLeader;

private:
	int32 MaxSearchResults;
	TSharedPtr<FOnlineSessionSearch> MatchSearch;
	TSharedPtr<FOnlineSessionSearch> OpenPublicSearch;
	
	void FindCompatibleMatchSessions();
	void OnFindMatchSessionsCompleted(bool bSuccess);
	UFUNCTION()
	void OnFindOpenPublicSessionsCompleted(const bool bSuccess);
	void UpdateSessionSettings(FOnlineSessionSettings* NewSessionSettings) const;
	FOnlineSessionSettings* GetSessionSettings() const;

	FDelegateHandle OpenPublicSessionsDelegateHandle;
	FDelegateHandle MatchSessionsDelegateHandle;
};
