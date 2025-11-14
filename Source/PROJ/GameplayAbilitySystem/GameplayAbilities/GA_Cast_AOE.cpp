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


	UE_LOG(LogTemp, Warning, TEXT("Ability is activated"));
	UAbilityTask_WaitTargetData* Task =
		UAbilityTask_WaitTargetData::WaitTargetData(
		this,
		TEXT("WaitTargetData"),
		EGameplayTargetingConfirmation::UserConfirmed,
		IndicatorActorClass
		);
	AGameplayAbilityTargetActor* DecalObj = GetWorld()->SpawnActor<AGameplayAbilityTargetActor>(IndicatorActorClass, GetAvatarActorFromActorInfo()->GetActorLocation(), GetAvatarActorFromActorInfo()->GetActorRotation() );
	if (DecalObj)
	{
		
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Failed to spawn DecalObj"));
	}
	Task->ValidData.AddDynamic(this, &UGA_Cast_AOE::OnTargetDataReceived);
	Task->Cancelled.AddDynamic(this, &UGA_Cast_AOE::OnTargetDataCancelled);

	Task->ReadyForActivation();

}

void UGA_Cast_AOE::InitializeTargetActor(class AGameplayAbilityTargetActor* SpawnedActor) const
{
	UE_LOG(LogTemp, Warning, TEXT("SpawnedActor is %s"), *SpawnedActor->GetName());
	if (AGATA_GroundTrace_Indicator* TA = Cast<AGATA_GroundTrace_Indicator>(SpawnedActor))
	{
		TA->Decal->DecalSize = FVector(Radius, Radius, 1.0f);
		
	}
}

void UGA_Cast_AOE::OnTargetDataReceived(const FGameplayAbilityTargetDataHandle& Data)
{
	UE_LOG(LogTemp, Warning, TEXT("OnTargetDataReceived"));
}

void UGA_Cast_AOE::OnTargetDataCancelled(const FGameplayAbilityTargetDataHandle& Data)
{
	UE_LOG(LogTemp, Warning, TEXT("OnTargetDataCancelled"));
}
