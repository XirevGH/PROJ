// Fill out your copyright notice in the Description page of Project Settings.


#include "AnimNotifyWithTag.h"

#include "PROJ/Characters/BaseCharacter.h"
#include "PROJ/GameplayAbilitySystem/BaseAbilitySystemComponent.h"

void UAnimNotifyWithTag::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference)
{
	if (!MeshComp) return;

	AActor* Actor = MeshComp->GetOwner();
	if (!Actor) return;

	if (ABaseCharacter* Player = Cast<ABaseCharacter>(Actor))
	{
		if (!Player) return;
		UAbilitySystemComponent* ASC = Player->GetAbilitySystemComponent();
		if (!ASC) return;
		
		FGameplayEventData EventData;
		EventData.Instigator = Player;
		EventData.Target = Player;
		
		ASC->HandleGameplayEvent(NotifyTag, &EventData);
	}
	
}
