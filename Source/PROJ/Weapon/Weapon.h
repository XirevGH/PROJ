// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Weapon.generated.h"

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
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Collider")
	UCapsuleComponent* Collider;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Owner")
	ABaseCharacter* WeaponOwner;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Socketname")
	FName SocketName;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="StartTrace")
	USceneComponent* StartTrace;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="EndTrace")
	USceneComponent* EndTrace;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Attachment")
	FVector LocationOffset;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Attachment")
	FRotator RotationOffset;
	
	bool bIsHitscanActive;
	
	float HitScanInterval = 1.f/30.f;
	
	FTimerHandle HitScanTimerHandle;
	
	
	void AttachToCharacter(class ACharacter* NewOwner, FName SocketName = NAME_None);
	UFUNCTION(BlueprintCallable)
	void HitScan();
	void HitScanStart(float Interval = 1.f/30.f);
	void HitScanEnd();
protected:
	
	virtual void BeginPlay() override;
	

};
