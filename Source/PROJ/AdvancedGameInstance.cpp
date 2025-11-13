#include "AdvancedGameInstance.h"

UAdvancedGameInstance::UAdvancedGameInstance(const FObjectInitializer& ObjectInitializer) :
	Super(ObjectInitializer)
{
	bAutoTravelOnAcceptedUserInviteReceived = false;
}
