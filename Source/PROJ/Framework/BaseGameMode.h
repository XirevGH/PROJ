#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameMode.h"
#include "BaseGameMode.generated.h"

class UCharacterClassInfo;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnTeamEmptyDelegate, FString, EmptyTeamID);
/**
 * 
 */
UCLASS()
class PROJ_API ABaseGameMode : public AGameMode
{
	GENERATED_BODY()

public:
	UFUNCTION()
	UCharacterClassInfo* GetCharacterClassDefaultInfo() const;

	UFUNCTION(BlueprintCallable, Category = "Teams")
	void AddPlayerToTeam(FString TeamID, APlayerController* Player);

	UFUNCTION(BlueprintCallable, Category = "Teams")
	void RemovePlayerFromTeam(FString TeamID, APlayerController* Player);

	UFUNCTION(BlueprintCallable, Category = "Teams")
	TArray<APlayerController*> GetPlayersInTeam(FString TeamID);

	UFUNCTION(BlueprintCallable, Category = "Teams")
	int32 GetTeamSize(FString TeamID);

	UPROPERTY(BlueprintAssignable, Category = "Teams")
	FOnTeamEmptyDelegate OnTeamEmpty;

	virtual void HandleSeamlessTravelPlayer(AController*& C) override;

protected:
	UFUNCTION(BlueprintImplementableEvent, Category = "Seamless Travel")
	void OnPlayerArrived(AController* C);
	
private:
	
	UPROPERTY(EditDefaultsOnly, Category = "Custom Values|Class Defaults")
	TObjectPtr<UCharacterClassInfo> ClassDefaults;

	TMap<FString, TArray<APlayerController*>> PlayerTeamMap;
};
