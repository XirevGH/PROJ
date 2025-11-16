

#include "Weapon.h"
#include "AbilitySystemComponent.h"
#include "GameplayEffect.h"
#include "GameplayEffectTypes.h"
#include "AbilitySystemGlobals.h"
#include "Components/CapsuleComponent.h"
#include "PROJ/BaseCharacter.h"
#include "PROJ/GameplayAbilitySystem/GameplayAbilities/BaseAttack.h"
#include "PROJ/GameplayAbilitySystem/GameplayAbilities/BaseGameplayAbility.h"

AWeapon::AWeapon()
{
	PrimaryActorTick.bCanEverTick = false;
	bReplicates = true;
	
	RootComponent = CreateDefaultSubobject<USceneComponent>("RootComponent");
	
	Mesh = CreateDefaultSubobject<UStaticMeshComponent>("Mesh");
	Mesh->SetupAttachment(RootComponent);
	
	Mesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	Mesh->SetCollisionResponseToAllChannels(ECR_Ignore);
	
	StartTrace = CreateDefaultSubobject<USceneComponent>("StartTrace");
	StartTrace->SetupAttachment(RootComponent);
	
	EndTrace = CreateDefaultSubobject<USceneComponent>("EndTrace");
	EndTrace->SetupAttachment(RootComponent);
	
	Collider = CreateDefaultSubobject<UCapsuleComponent>("Collider");
	Collider->SetupAttachment(RootComponent);
	
	Collider->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	Collider->SetCollisionResponseToAllChannels(ECR_Ignore);
	
	LocationOffset = FVector::ZeroVector;
	RotationOffset = FRotator::ZeroRotator;

}

void AWeapon::ApplyEffectToTarget(AActor* Target)
{
	if (!HasAuthority() || !Target) return;
	
	UAbilitySystemComponent* TargetASC = UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(Target);
	if (!TargetASC) return;

	UAbilitySystemComponent* OwnerASC = UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(GetOwner());
	if (!OwnerASC) return;

	if (!Ability)
	{
		UE_LOG(LogTemp, Warning, TEXT("Weapon has no Ability assigned!"));
		return;
	}

	UE_LOG(LogTemp, Warning, TEXT("Applying effects to target: %s"), *Target->GetName());
	
	for (auto& EffectClass : Effects)
	{
		if (!EffectClass)
		{
			UE_LOG(LogTemp, Warning, TEXT("EffectClass is null!"));
			continue;
		}
		
		if (!EffectClass) continue;
		FGameplayEffectContextHandle EffectContext = OwnerASC->MakeEffectContext();
		FGameplayEffectSpecHandle SpecHandle = OwnerASC->MakeOutgoingSpec(EffectClass,1.f, EffectContext);

		if (SpecHandle.IsValid())
		{
			FGameplayTag DamageTag = FGameplayTag::RequestGameplayTag(FName("Data.Damage"));
			SpecHandle.Data->SetSetByCallerMagnitude(DamageTag,Ability->BaseDamage);

			TargetASC->ApplyGameplayEffectSpecToSelf(*SpecHandle.Data.Get());
		}
		
	}
}

void AWeapon::AttachToCharacter(ACharacter* NewOwner, FName InSocketName)
{
	WeaponOwner = Cast<ABaseCharacter>(NewOwner);
	if (WeaponOwner && Mesh)
		
	Mesh->SetRelativeLocation(LocationOffset);
	Mesh->SetRelativeRotation(RotationOffset);
	
	AttachToComponent(
		WeaponOwner->GetMesh(),
		FAttachmentTransformRules::KeepRelativeTransform,
		InSocketName);
	
	
}

void AWeapon::HitScan()
{
	if (!StartTrace || !EndTrace) return;
	
	FVector const Start = StartTrace->GetComponentLocation();
	FVector const End = EndTrace->GetComponentLocation();
	
	FCollisionQueryParams TraceParams(FName(TEXT("WeaponTrace")), true, this);
	TraceParams.bReturnPhysicalMaterial = false;
	
	TArray<FHitResult> HitResults;
	
	bool bHit = GetWorld()->LineTraceMultiByChannel(
	HitResults,
	Start,
	End,
	ECC_Pawn,
	TraceParams);
	
	if (bHit)
	{
		for (auto& Hit : HitResults)
		{
			AActor* HitActor = Hit.GetActor();
			if (HitActor)
			UE_LOG(LogTemp, Warning, TEXT("Hit Actor: %s"), *HitActor->GetName());
			ApplyEffectToTarget(HitActor);
			
		}
	}
	DrawDebugLine(GetWorld(), Start, End, FColor::Green, false, 2.f, 0, 2.f);
	for (auto& Hit : HitResults)
	{
		DrawDebugSphere(GetWorld(), Hit.ImpactPoint, 5.f, 8, FColor::Red, false, 2.f);
	}

}

void AWeapon::HitScanStart(float Interval)
{
	if(bIsHitscanActive) return;
	
	bIsHitscanActive = true;
	
	HitScan();
	
	GetWorld()->GetTimerManager().SetTimer(
		HitScanTimerHandle,
		this,
		&AWeapon::HitScan,
		Interval,
		true);
}

void AWeapon::HitScanEnd()
{
	if (!bIsHitscanActive) return;

	bIsHitscanActive = false;
	
	GetWorld()->GetTimerManager().ClearTimer(HitScanTimerHandle);
}

void AWeapon::BeginPlay()
{
	Super::BeginPlay();
	
	WeaponOwner = Cast<ABaseCharacter>(GetOwner());
	if (WeaponOwner && !SocketName.IsNone())
	AttachToCharacter(WeaponOwner, SocketName);
}

