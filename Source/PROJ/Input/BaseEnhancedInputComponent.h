// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BaseInputConfig.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInput/Public/InputTriggers.h"
#include "BaseEnhancedInputComponent.generated.h"

class FEnhancedInputActionBinding;
/**
 * 
 */
UCLASS()
class PROJ_API UBaseEnhancedInputComponent : public UEnhancedInputComponent
{
	GENERATED_BODY()

public:

	template<
		class UserClass,
		typename StartedFunc,
		typename OngoingFunc,
		typename TriggeredFunc,
		typename CompletedFunc,
		typename CanceledFunc,
		typename TapFunc>
	void BindAbilityActions(
		UBaseInputConfig* InputConfig,
		UserClass* Object,
		StartedFunc Started,
		OngoingFunc Ongoing,
		TriggeredFunc Triggered,
		CompletedFunc Completed,
		CanceledFunc Canceled,
		TapFunc OnTap);
};

template <class UserClass,
		typename StartedFunc,
		typename OngoingFunc,
		typename TriggeredFunc,
		typename CompletedFunc,
		typename CanceledFunc,
		typename TapFunc>
void UBaseEnhancedInputComponent::BindAbilityActions(
	UBaseInputConfig* InputConfig,
	UserClass* Object,
	StartedFunc Started,
	OngoingFunc Ongoing,
	TriggeredFunc Triggered,
	CompletedFunc Completed,
	CanceledFunc Canceled,
	TapFunc OnTap)
{
	check(InputConfig);
	
	
	for (const FBaseInputAction& Action : InputConfig->BaseInputActions)
	{
		/*if (!IsValid(Action.InputAction) || !Action.InputTag.IsValid())
			continue;*/

		/*Normal inputs*/
		if (IsValid(Action.InputAction))
		{
			if (Started)
				BindAction(Action.InputAction, ETriggerEvent::Started, Object, Started, Action.InputTag);
			if (Ongoing)
				BindAction(Action.InputAction, ETriggerEvent::Ongoing, Object,Ongoing , Action.InputTag);
			if (Triggered)
				BindAction(Action.InputAction, ETriggerEvent::Triggered, Object, Triggered, Action.InputTag);
			if (Completed)
				BindAction(Action.InputAction, ETriggerEvent::Completed, Object, Completed, Action.InputTag);
			if (Canceled)
				BindAction(Action.InputAction, ETriggerEvent::Canceled, Object, Canceled, Action.InputTag);
		}
		/*Tap input*/
		if (OnTap && IsValid(Action.TapInputAction))
		{
			BindAction(Action.TapInputAction, ETriggerEvent::Triggered, Object, OnTap, Action.InputTag);
		}
	}
}
