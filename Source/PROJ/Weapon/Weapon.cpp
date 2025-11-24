

#include "Weapon.h"
#include "AbilitySystemComponent.h"
#include "GameplayEffect.h"
#include "GameplayEffectTypes.h"
#include "AbilitySystemGlobals.h"
#include "Components/CapsuleComponent.h"
#include "PROJ/Characters/BaseCharacter.h"
#include "PROJ/Data/AttackData.h"
#include "PROJ/GameplayAbilitySystem/GameplayAbilities/BaseAttack.h"
#include "PROJ/GameplayAbilitySystem/GameplayAbilities/BaseGameplayAbility.h"

AWeapon::AWeapon()
{
	PrimaryActorTick.bCanEverTick = false;
	bReplicates = true;
	bNetUseOwnerRelevancy = true;
	SetReplicateMovement(true);
	
	RootComponent = CreateDefaultSubobject<USceneComponent>("RootComponent");
	
	Mesh = CreateDefaultSubobject<UStaticMeshComponent>("Mesh");
	Mesh->SetupAttachment(RootComponent);
	Mesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	Mesh->SetCollisionResponseToAllChannels(ECR_Ignore);
	
	StartTrace = CreateDefaultSubobject<USceneComponent>("StartTrace");
	StartTrace->SetupAttachment(RootComponent);
	
	EndTrace = CreateDefaultSubobject<USceneComponent>("EndTrace");
	EndTrace->SetupAttachment(RootComponent);
	
}

void AWeapon::OnRep_Owner()
{
	Super::OnRep_Owner();
	AttachWeapon();
}

void AWeapon::OnRep_AttachmentReplication()
{
	Super::OnRep_AttachmentReplication();
	AttachWeapon();
}
void AWeapon::AttachWeapon()
{
	if (ACharacter* Char = Cast<ACharacter>(GetOwner()))
	{
		AttachToComponent(Char->GetMesh(),
		FAttachmentTransformRules::SnapToTargetNotIncludingScale,
		TEXT("WeaponSocket"));

		Mesh->SetRelativeLocation(LocationOffset);
		Mesh->SetRelativeRotation(RotationOffset);
	}
}
void AWeapon::ApplyEffectToTarget(AActor* Target)
{
	if (!HasAuthority() || !Target || !Ability) return;
	
	UAbilitySystemComponent* TargetASC = UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(Target);
	UAbilitySystemComponent* OwnerASC = UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(GetOwner());
	
	if (!OwnerASC || !TargetASC) return;

	UAttackData* Data = Ability->GetAttackData();
	
	for (auto& EffectClass : Data->Effects)
	{
		if (!EffectClass) continue;
		FGameplayEffectContextHandle EffectContext = OwnerASC->MakeEffectContext();
		FGameplayEffectSpecHandle SpecHandle = OwnerASC->MakeOutgoingSpec(EffectClass,1.f, EffectContext);

		if (SpecHandle.IsValid())
		{
			FGameplayTag DamageTag = FGameplayTag::RequestGameplayTag(FName("Data.Damage"));
			
			for (FGameplayEffectSpecHandle Spec : Ability->MakeEffectSpecsHandles())
			{
				TargetASC->ApplyGameplayEffectSpecToSelf(*Spec.Data.Get());
			}
			
		}
		
	}
}

void AWeapon::HitScanStart(float Interval)
{
	
	if (!HasAuthority() || bIsHitscanActive) return;
	
	Targets.Empty();
	
	GetWorld()->GetTimerManager().SetTimer(
		HitScanTimerHandle,
		this,
		&AWeapon::HitScan,
		Interval,
		true);
	
	bIsHitscanActive = true;
}

void AWeapon::HitScan()
{
	if (!StartTrace || !EndTrace || !HasAuthority()) return;
	
	FVector const Start = StartTrace->GetComponentLocation();
	FVector const End = EndTrace->GetComponentLocation();
	
	FCollisionQueryParams TraceParams(FName(TEXT("WeaponTrace")), true, this);
	TraceParams.bReturnPhysicalMaterial = false;
	TraceParams.AddIgnoredActor(this);
	TraceParams.AddIgnoredActor(GetOwner());
	
	TArray<FHitResult> HitResults;
	if (GetWorld()->LineTraceMultiByChannel(HitResults, Start,End, ECC_Pawn,TraceParams))
	{
		for (auto& Hit : HitResults)
		{
			AActor* HitActor = Hit.GetActor();
			if (!HitActor || Targets.Contains(HitActor)) continue;

			Targets.Add(HitActor);
			ApplyEffectToTarget(HitActor);
		}
	}
#if	WITH_EDITOR
	DrawDebugLine(GetWorld(), Start, End, FColor::Green, false, 2.f, 0, 2.f);
	for (auto& Hit : HitResults)
	{
		DrawDebugSphere(GetWorld(), Hit.ImpactPoint, 5.f, 8, FColor::Red, false, 2.f);
	}
#endif
}

void AWeapon::HitScanEnd()
{
	if (!HasAuthority() || !bIsHitscanActive) return;

	Targets.Empty();
	if (!bIsHitscanActive && !GetWorld()->GetTimerManager().IsTimerActive(HitScanTimerHandle)) return;
	GetWorld()->GetTimerManager().ClearTimer(HitScanTimerHandle);
	
	bIsHitscanActive = false;
}

void AWeapon::Server_HitScanStart_Implementation(float DeltaTime)
{
	HitScanStart(DeltaTime);

}

