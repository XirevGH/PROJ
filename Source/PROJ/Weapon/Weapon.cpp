

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
	
	Collider = CreateDefaultSubobject<UCapsuleComponent>("Collider");
	Collider->SetupAttachment(RootComponent);
	
	Collider->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	Collider->SetCollisionResponseToAllChannels(ECR_Ignore);
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
void AWeapon::HitScan()
{
	UE_LOG(LogTemp, Verbose, TEXT("HitScan() tick on %s by %s. Targets currently: %d"),
	*GetName(),
	HasAuthority() ? TEXT("SERVER") : TEXT("CLIENT"),
	Targets.Num());
	
	if (!HasAuthority()) return;
	
	if (!StartTrace || !EndTrace) return;
	
	FVector const Start = StartTrace->GetComponentLocation();
	FVector const End = EndTrace->GetComponentLocation();
	
	FCollisionQueryParams TraceParams(FName(TEXT("WeaponTrace")), true, this);
	TraceParams.bReturnPhysicalMaterial = false;

	TraceParams.AddIgnoredActor(this);
	TraceParams.AddIgnoredActor(GetOwner());
	
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
			if (!HitActor) continue;
			
			if (!Targets.Contains(HitActor))
			{
				Targets.Add(HitActor);
				ApplyEffectToTarget(HitActor);
			}
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
	UE_LOG(LogTemp, Warning, TEXT("HitScanStart called on %s. HasAuthority(): %s. TimerActive: %s"),
	*GetName(),
	HasAuthority() ? TEXT("SERVER") : TEXT("CLIENT"),
	GetWorld()->GetTimerManager().IsTimerActive(HitScanTimerHandle) ? TEXT("true") : TEXT("false"));
	
	if (!HasAuthority()) return;
	
	if(bIsHitscanActive) return;
	
	Targets.Empty();
	
	//HitScan();
	
	GetWorld()->GetTimerManager().SetTimer(
		HitScanTimerHandle,
		this,
		&AWeapon::HitScan,
		Interval,
		true);
	bIsHitscanActive = true;
}

void AWeapon::HitScanEnd()
{
	if (!HasAuthority()) return;
	
	if (!bIsHitscanActive) return;

	if (!bIsHitscanActive && !GetWorld()->GetTimerManager().IsTimerActive(HitScanTimerHandle)) return;
	
	Targets.Empty();
	GetWorld()->GetTimerManager().ClearTimer(HitScanTimerHandle);
	bIsHitscanActive = false;
}

void AWeapon::Server_HitScanStart_Implementation(float DeltaTime)
{
	HitScanStart(DeltaTime);

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

void AWeapon::BeginPlay()
{
	Super::BeginPlay();
	
}

