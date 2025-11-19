// Fill out your copyright notice in the Description page of Project Settings.


#include "GA_Cast_AOE.h"
#include "Abilities/Tasks/AbilityTask_WaitConfirmCancel.h"
#include "Components/DecalComponent.h"
#include "PROJ/GameplayAbilitySystem/Indicators/Indicator.h"


#include "GameFramework/PlayerController.h"

void UGA_Cast_AOE::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
                                   const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);
	CachedHandle = Handle;
	CachedActorInfo = ActorInfo;
	CachedActivationInfo = ActivationInfo;

	
	FActorSpawnParameters SpawnParams;
	SpawnParams.Owner = ActorInfo->AvatarActor.Get();
	SpawnParams.Instigator = Cast<APawn>(ActorInfo->AvatarActor.Get());
	Indicator = GetWorld()->SpawnActor<AIndicator>(IndicatorClass, ActorInfo->AvatarActor->GetActorLocation(), FRotator::ZeroRotator, SpawnParams);
	
	APawn* Pawn = Cast<APawn>(ActorInfo->AvatarActor);
	APlayerController* PC = Pawn ? Cast<APlayerController>(Pawn->GetController()) : nullptr;
	 if (!PC)
	 {
	 	UE_LOG(LogTemp, Warning, TEXT("no controller"));
	 	return;
	 }
	
	if (Indicator)
	{
		Indicator->CasterController = PC;
		Indicator->Caster = Cast<AActor>(ActorInfo->AvatarActor);
		Indicator->MaxRange = Range;
		Indicator->Decal->DecalSize = FVector(128, Radius, Radius);
	}
	
	UAbilityTask_WaitConfirmCancel* Task = UAbilityTask_WaitConfirmCancel::WaitConfirmCancel(this);
	Task->OnConfirm.AddDynamic(this, &UGA_Cast_AOE::OnConfirm);
	Task->OnCancel.AddDynamic(this, &UGA_Cast_AOE::OnCancel);
	Task->ReadyForActivation();
	//Indicator->SetActorHiddenInGame(true);
	UE_LOG(LogTemp, Warning, TEXT("Task %s"), Task->IsValidLowLevel() ? TEXT("Successfully activated") : TEXT("Failed to activate"));

}

void UGA_Cast_AOE::OnCancel_Implementation()
{
	//Indicator->SetActorHiddenInGame(true);
	Indicator->Destroy();
	EndAbility(CachedHandle, CachedActorInfo, CachedActivationInfo, true, true);
	UE_LOG(LogTemp, Warning, TEXT("task canel"));
}

void UGA_Cast_AOE::OnConfirm_Implementation()
{
	UE_LOG(LogTemp, Warning, TEXT("task actiavte"));
	
	SpawnLocation = Indicator->GetActorLocation();
	if (GetOwningActorFromActorInfo()->HasAuthority())  // only server
	{
		UE_LOG(LogTemp, Warning, TEXT("SpawnLocation: %s"), *SpawnLocation.ToString());
		CommitAbility(CachedHandle, CachedActorInfo, CachedActivationInfo);
	}
	
	Indicator->Destroy();
}


