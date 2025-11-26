// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "AnimNotifies/AnimNotify_PlayMontageNotify.h"
#include "AnimNotifyWithTag.generated.h"

/**
 * 
 */
UCLASS()
class PROJ_API UAnimNotifyWithTag : public UAnimNotify_PlayMontageNotify
{
	GENERATED_BODY()
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Gameplay")
	FGameplayTag NotifyTag;

	virtual void Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference) override;
};
