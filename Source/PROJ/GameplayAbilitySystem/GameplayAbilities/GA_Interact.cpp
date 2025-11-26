// Fill out your copyright notice in the Description page of Project Settings.


#include "GA_Interact.h"

#include "PROJ/Interfaces/Interactable.h"

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

	AActor* Target = Hit.GetActor();
	UActorComponent* Component = Hit.GetComponent();
	UE_LOG(LogTemp, Error, TEXT("Hit Actor: %s"), 
	Hit.GetActor() ? *Hit.GetActor()->GetName() : TEXT("NULL"));

	UE_LOG(LogTemp, Error, TEXT("Hit Component: %s"), 
		Hit.GetComponent() ? *Hit.GetComponent()->GetName() : TEXT("NULL"));

	UE_LOG(LogTemp, Error, TEXT("Hit PhysMaterial: %s"),
		Hit.PhysMaterial.IsValid() ? *Hit.PhysMaterial->GetName() : TEXT("NULL"));
	
	
	if (!Target )
	{
		//UE_LOG(LogTemp, Error, TEXT("Target or Component is null"));
		CancelAbility(Handle, ActorInfo, ActivationInfo, true);
	}

	// Client calls Server
	
		Server_Interact(Target, Component );
	

	EndAbility(Handle, ActorInfo, ActivationInfo, true, false);
}


void UGA_Interact::Server_Interact_Implementation(AActor* Target, UActorComponent* Component)
{
	if (!Target )
	{
		UE_LOG(LogTemp, Error, TEXT("Target or Component is null"));
		return;
	}
	if (Target->Implements<UInteractable>())
	{
		IInteractable::Execute_Interact(Target, GetAvatarActorFromActorInfo(), Component);
		UE_LOG(LogTemp, Error, TEXT("Call interact"));
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("Target not  Implements UInteractable"));
	}
}