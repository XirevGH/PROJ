// Fill out your copyright notice in the Description page of Project Settings.


#include "GA_Interact.h"

void UGA_Interact::OnGiveAbility(const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilitySpec& Spec)
{
	Super::OnGiveAbility(ActorInfo, Spec);

	InputTag = FGameplayTag::RequestGameplayTag("Input.Ability.Interact");
}

void UGA_Interact::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);
	UE_LOG(LogTemp, Warning, TEXT("Active Ability"));
	
	APawn* Pawn = Cast<APawn>(GetAvatarActorFromActorInfo());
	APlayerController* CasterController = Pawn ? Cast<APlayerController>(Pawn->GetController()) : nullptr;
	if (!CasterController)
	{
		UE_LOG(LogTemp, Error, TEXT("No APlayerController"));
		CancelAbility(Handle, ActorInfo, ActivationInfo, true);
	}
	
	FHitResult Hit;
	CasterController->GetHitResultUnderCursor(ECC_GameTraceChannel2, false, Hit);
	
	AActor* Target = const_cast<AActor*>(TriggerEventData->Target.Get());

	if (!Target)
	{
		CancelAbility(Handle, ActorInfo, ActivationInfo, true);
		return;
	}

	// Client calls Server
	if (ActorInfo->IsLocallyControlled())
	{
		//ServerInteract(Target);
	}

	EndAbility(Handle, ActorInfo, ActivationInfo, true, false);
}
