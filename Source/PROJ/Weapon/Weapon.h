// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Weapon.generated.h"

class UBaseAttack;
class UBaseGameplayAbility;
class UGameplayEffect;
class ABaseCharacter;
class UCapsuleComponent;

UCLASS()
class PROJ_API AWeapon : public AActor
{
	GENERATED_BODY()
	
public:	
	
	AWeapon();
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Mesh")
	UStaticMeshComponent* Mesh;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Owner")
	ABaseCharacter* WeaponOwner;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Socketname")
	FName SocketName;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="StartTrace")
	USceneComponent* StartTrace;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="EndTrace")
	USceneComponent* EndTrace;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Attachment")
	FVector LocationOffset = FVector(-9.f, 1.f, 8.f);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Attachment")
	FRotator RotationOffset = FRotator(180.f, -90.f, 90.f);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Ability")
	UBaseAttack* Ability;

	virtual void OnRep_Owner() override;

	virtual void OnRep_AttachmentReplication() override;

	FTimerHandle AttachTimerHandle;
	
	UPROPERTY()
	TSet<AActor*> Targets;
	
	bool bIsHitscanActive;
	
	float HitScanInterval = 1.f/30.f;
	
	FTimerHandle HitScanTimerHandle;

	UPROPERTY(EditDefaultsOnly, Category="Effects")
	TArray<TSubclassOf<UGameplayEffect>> Effects;
	
	UFUNCTION(BlueprintCallable)
	void HitScan();
	UFUNCTION()
	void HitScanStart(float Interval = 1.f/30.f);
	UFUNCTION()
	void HitScanEnd();
	UFUNCTION(Server, Reliable)
	void Server_HitScanStart(float Interval = 0.03f);
	void AttachWeapon();

};
