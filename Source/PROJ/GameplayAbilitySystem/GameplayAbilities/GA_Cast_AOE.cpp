// Fill out your copyright notice in the Description page of Project Settings.


#include "GA_Cast_AOE.h"
#include "Abilities/Tasks/AbilityTask_WaitTargetData.h"
#include "Abilities/GameplayAbilityTargetActor.h"

void UGA_Cast_AOE::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);


  /*
  UAbilityTask_WaitTargetData* Task =
		UAbilityTask_WaitTargetData::WaitTargetData(
			this,
			TEXT("WaitTargetData"),
			EGameplayTargetingConfirmation::Instant,
			TargetActorClass
		);

	Task->CollisionRadius = CollisionRadius;
	Task->CollisionHeight = CollisionHeight;
	Task->MaxRange = MaxRange;

	Task->TraceProfile.Name = TEXT("ProjectileTrace");
	Task->bTraceAffectsAimPitch = true;
	Task->bDebug = bDebug;

	Task->StartLocation = StartLocationInfo;
	Task->Filter = FilterHandle;
	Task->ReticleParams = ReticleParams;
	Task->ReticleClass = ReticleClass;

	Task->ValidData.AddDynamic(this, &UGA_CastProjectile::OnTargetDataReceived);
	Task->Cancelled.AddDynamic(this, &UGA_CastProjectile::OnTargetDataCancelled);

	Task->ReadyForActivation();
	*/

}
