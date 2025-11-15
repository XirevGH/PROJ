#pragma once

#include "CoreMinimal.h"
#include "AdvancedFriendsGameInstance.h"
#include "AdvancedFriendsGameInstance.h"
#include "AdvancedGameInstance.generated.h"

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
	
	void FindCompatibleMatchSessions();
	void OnFindMatchSessionsCompleted(bool bSuccess);
	
	FOnlineSessionSettings* GetSessionSettings() const;
	void UpdateSessionSettings(FOnlineSessionSettings* NewSessionSettings) const;
};
