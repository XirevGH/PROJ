// Fill out your copyright notice in the Description page of Project Settings.


#include "GA_Cast_AOE.h"
#include "Abilities/Tasks/AbilityTask_WaitTargetData.h"
// #include "GameFramework/PlayerController.h"
// #include "PROJ/GameplayAbilitySystem/Indicators/BaseAbilityTask_WaitTargetData.h"
#include "AbilitySystemBlueprintLibrary.h"
void UGA_Cast_AOE::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
                                   const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);
	
	// UAbilityTask_WaitTargetData* Task = UAbilityTask_WaitTargetData::WaitTargetData(
	// this,
	// FName("WaitForTarget"),
	// EGameplayTargetingConfirmation::UserConfirmed,
	// IndicatorClass);
	//
	// Task->ValidData.AddDynamic(this, &UGA_Cast_AOE::OnTargetDataReceived);
	// Task->Cancelled.AddDynamic(this, &UGA_Cast_AOE::OnTargetDataCancelled);
	// Task->ReadyForActivation();
	

	//UE_LOG(LogTemp, Warning, TEXT("Task %s"), Task->IsValidLowLevel() ? TEXT("Successfully activated") : TEXT("Failed to activate"));

}

void UGA_Cast_AOE::OnCancel_Implementation(const FGameplayAbilityTargetDataHandle& Data)
{
	EndAbility(CachedHandle, CachedActorInfo, CachedActivationInfo, true, true);
	UE_LOG(LogTemp, Warning, TEXT("task cancel"));
}

void UGA_Cast_AOE::OnConfirm_Implementation(const FGameplayAbilityTargetDataHandle& Data)
{
}

void UGA_Cast_AOE::OnTargetDataReceived(const FGameplayAbilityTargetDataHandle& Data)
{
	CommitAbility(CachedHandle, CachedActorInfo, CachedActivationInfo);
	UE_LOG(LogTemp, Warning, TEXT("task activate"));
	//FTransform EndPoint = UAbilitySystemBlueprintLibrary::GetTargetDataEndPointTransform(Data, 0);
	OnConfirm_Implementation(Data);
}

void UGA_Cast_AOE::OnTargetDataCancelled(const FGameplayAbilityTargetDataHandle& Data)
{
	OnCancel_Implementation(Data);
	UE_LOG(LogTemp, Warning, TEXT("task cancel"));
}


