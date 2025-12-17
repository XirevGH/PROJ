#include "LobbyGameMode.h"

#include "Kismet/GameplayStatics.h"
#include "PROJ/GameSession/EOSGameInstance.h"

void ALobbyGameMode::PreLogin(const FString& Options, const FString& Address, const FUniqueNetIdRepl& UniqueId,
                              FString& ErrorMessage)
{
	Super::PreLogin(Options, Address, UniqueId, ErrorMessage);

	if (!ErrorMessage.IsEmpty()) return;

	UEOSGameInstance* GI = Cast<UEOSGameInstance>(GetGameInstance());
	if (!GI) return;

	ESessionState CurrentState = GI->GetSessionState();
	FString JoinIntent = UGameplayStatics::ParseOption(Options, KEY_JOIN_INTENT);
	UE_LOG(LogTemp, Warning, TEXT("PreLogin: State=[%d], Intent=[%s]"), (int32)CurrentState, *JoinIntent);

	if (CurrentState == ESessionState::SearchingForMatch && JoinIntent == JOIN_INTENT_LOBBY)
	{
		ErrorMessage = TEXT("Lobby is currently searching for a match. Lobby join blocked.");
		return;
	}

	if (CurrentState == ESessionState::Lobby && JoinIntent == JOIN_INTENT_MATCH)
	{
		ErrorMessage = TEXT("Match search was cancelled. Match join blocked.");
	}
}
