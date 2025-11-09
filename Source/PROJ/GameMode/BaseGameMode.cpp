// Fill out your copyright notice in the Description page of Project Settings.


#include "BaseGameMode.h"

UCharacterClassInfo* ABaseGameMode::GetCharacterClassDefaultInfo() const
{
	return ClassDefaults;
}
