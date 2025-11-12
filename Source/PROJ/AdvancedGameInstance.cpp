#include "AdvancedGameInstance.h"

UAdvancedGameInstance::UAdvancedGameInstance(const FObjectInitializer& ObjectInitializer) :
	Super(ObjectInitializer),
	OnDestroySessionCompleteDelegate(FOnDestroySessionCompleteDelegate::CreateUObject(this, &ThisClass::OnDestroySessionComplete))
{}

void UAdvancedGameInstance::Init()
{
	Super::Init();
}
