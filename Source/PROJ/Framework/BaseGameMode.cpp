#include "BaseGameMode.h"

UCharacterClassInfo* ABaseGameMode::GetCharacterClassDefaultInfo() const
{
	return ClassDefaults;
}

void ABaseGameMode::AddPlayerToTeam(FString TeamID, APlayerController* Player)
{
	if (!Player || !IsValid(Player))
	{
		UE_LOG(LogTemp, Warning, TEXT("Player to add is NULL or invalid"));
		return;
	}

	PlayerTeamMap.FindOrAdd(TeamID).AddUnique(Player);
	
	UE_LOG(LogTemp, Display, TEXT("Added %s to Team: %s"), *Player->GetName(), *TeamID);
}

void ABaseGameMode::RemovePlayerFromTeam(FString TeamID, APlayerController* Player)
{
	if (!Player)
	{
		UE_LOG(LogTemp, Warning, TEXT("Player to remove is NULL"));
		return;
	}

	if (TArray<APlayerController*>* TeamList = PlayerTeamMap.Find(TeamID))
	{
		TeamList->Remove(Player);
		
		if (TeamList->Num() == 0)
		{
			UE_LOG(LogTemp, Display, TEXT("Team %s is now empty! Broadcasting event."), *TeamID);
			
			if (OnTeamEmpty.IsBound())
			{
				OnTeamEmpty.Broadcast(TeamID);
			}

			// Remove from Map (Clean memory)
			// Do I want this?
			PlayerTeamMap.Remove(TeamID);
		}
	}
}

TArray<APlayerController*> ABaseGameMode::GetPlayersInTeam(FString TeamID)
{
	if (TArray<APlayerController*>* TeamList = PlayerTeamMap.Find(TeamID))
	{
		// Cleanup: Remove any players that disconnected (became invalid) since the last check
		for (int32 i = TeamList->Num() - 1; i >= 0; i--)
		{
			if (!IsValid((*TeamList)[i]))
			{
				TeamList->RemoveAt(i);
			}
		}
		
		return *TeamList;
	}
	return TArray<APlayerController*>();
}

int32 ABaseGameMode::GetTeamSize(FString TeamID)
{
	if (const TArray<APlayerController*>* TeamList = PlayerTeamMap.Find(TeamID))
	{
		return TeamList->Num();
	}
	
	UE_LOG(LogTemp, Warning, TEXT("Team %s does not exist"), *TeamID);
	return 0;
}

void ABaseGameMode::HandleSeamlessTravelPlayer(AController*& C)
{
	Super::HandleSeamlessTravelPlayer(C);
	OnPlayerArrived(C);
}
