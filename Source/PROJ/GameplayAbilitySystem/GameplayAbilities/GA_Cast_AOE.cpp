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

}

void UGA_Cast_AOE::OnCancel_Implementation(const FGameplayAbilityTargetDataHandle& Data)
{
	EndAbility(CachedHandle, CachedActorInfo, CachedActivationInfo, true, true);
	//UE_LOG(LogTemp, Warning, TEXT("task cancel"));
}

void UGA_Cast_AOE::OnConfirm_Implementation(const FGameplayAbilityTargetDataHandle& Data)
{
}

void UGA_Cast_AOE::OnTargetDataReceived(const FGameplayAbilityTargetDataHandle& Data)
{
	OnConfirm_Implementation(Data);
	//UE_LOG(LogTemp, Warning, TEXT("task activate"));
}

void UGA_Cast_AOE::OnTargetDataCancelled(const FGameplayAbilityTargetDataHandle& Data)
{
	OnCancel_Implementation(Data);
	//UE_LOG(LogTemp, Warning, TEXT("task cancel"));
}


