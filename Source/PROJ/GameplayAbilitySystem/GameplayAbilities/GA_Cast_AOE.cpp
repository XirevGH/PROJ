// Fill out your copyright notice in the Description page of Project Settings.


#include "GA_Cast_AOE.h"
#include "Abilities/Tasks/AbilityTask_WaitTargetData.h"
#include "Abilities/GameplayAbilityTargetActor.h"
#include "Components/DecalComponent.h"
#include "PROJ/GameplayAbilitySystem/Indicators/GATA_GroundTrace_Indicator.h"

#include "GameFramework/PlayerController.h"

void UGA_Cast_AOE::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
                                   const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);
	
}


void UGA_Cast_AOE::OnTargetDataReceived(const FGameplayAbilityTargetDataHandle& Data)
{
	UE_LOG(LogTemp, Warning, TEXT("OnTargetDataReceived"));
}

void UGA_Cast_AOE::OnTargetDataCancelled(const FGameplayAbilityTargetDataHandle& Data)
{
	UE_LOG(LogTemp, Warning, TEXT("OnTargetDataCancelled"));
}
