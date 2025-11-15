// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Weapon.generated.h"

class UGameplayEffect;
class ABaseCharacter;
class UCapsuleComponent;
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnWeaponHit, const TArray<FHitResult>&, HitResults);
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
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Weapon|Damage")
	TSubclassOf<UGameplayEffect> DamageEffectClass;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="HitActors")
	TArray<AActor*> HitActors;
	
	UPROPERTY(BlueprintAssignable)
	FOnWeaponHit OnWeaponHit;
	
	UFUNCTION(Server, Reliable)
	void ServerHitScan();
	
	void AttachToCharacter(class ACharacter* NewOwner, FName SocketName = NAME_None);
	UFUNCTION(BlueprintCallable)
	void HitScan();
	UFUNCTION(BlueprintCallable)
	void HitScanStart(float Interval, TSubclassOf<UGameplayEffect> DamageEffect);
	UFUNCTION(BlueprintCallable)
	void HitScanEnd();
protected:
	
	virtual void BeginPlay() override;
	

};
