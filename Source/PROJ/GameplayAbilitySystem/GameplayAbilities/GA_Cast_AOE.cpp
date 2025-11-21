// Fill out your copyright notice in the Description page of Project Settings.


#include "GA_Cast_AOE.h"
#include "Abilities/Tasks/AbilityTask_WaitTargetData.h"
#include "Components/DecalComponent.h"
#include "PROJ/GameplayAbilitySystem/Indicators/Indicator.h"


#include "GameFramework/PlayerController.h"

void UGA_Cast_AOE::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
                                   const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);
	
	UAbilityTask_WaitTargetData* Task = UAbilityTask_WaitTargetData::WaitTargetData(
	this,
	FName("WaitForTarget"),
	EGameplayTargetingConfirmation::UserConfirmed,
	IndicatorClass);
	//Task->SetTargetActor();
	Task->ValidData.AddDynamic(this, &UGA_Cast_AOE::OnTargetDataReceived);
	Task->Cancelled.AddDynamic(this, &UGA_Cast_AOE::OnTargetDataCancelled);
	Task->ReadyForActivation();
	//Task->BeginSpawningActor();
	//UAbilityTask_WaitConfirmCancel* Task = UAbilityTask_WaitConfirmCancel::WaitConfirmCancel(this);
	//Task->OnConfirm.AddDynamic(this, &UGA_Cast_AOE::OnConfirm);
	//Task->OnCancel.AddDynamic(this, &UGA_Cast_AOE::OnCancel);
	//Task->ReadyForActivation();
	//Indicator->SetActorHiddenInGame(true);
	UE_LOG(LogTemp, Warning, TEXT("Task %s"), Task->IsValidLowLevel() ? TEXT("Successfully activated") : TEXT("Failed to activate"));

}

void UGA_Cast_AOE::OnCancel_Implementation()
{
	//Indicator->SetActorHiddenInGame(true);
	//Indicator->Destroy();
	EndAbility(CachedHandle, CachedActorInfo, CachedActivationInfo, true, true);
	UE_LOG(LogTemp, Warning, TEXT("task cancel"));
}

void UGA_Cast_AOE::OnConfirm_Implementation()
{
	
	
		//FVector Location = Indicator->GetActorLocation();
		//ServerConfirmTarget(Location);
	//	Indicator->Destroy();
	
}

void UGA_Cast_AOE::OnTargetDataReceived(const FGameplayAbilityTargetDataHandle& Data)
{
	CommitAbility(CachedHandle, CachedActorInfo, CachedActivationInfo);
	UE_LOG(LogTemp, Warning, TEXT("task activate"));
}

void UGA_Cast_AOE::OnTargetDataCancelled(const FGameplayAbilityTargetDataHandle& Data)
{
	UE_LOG(LogTemp, Warning, TEXT("task cancel"));
}


