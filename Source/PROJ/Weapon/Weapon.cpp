

#include "Weapon.h"

#include "AbilitySystemGlobals.h"
#include "Components/CapsuleComponent.h"
#include "PROJ/BaseCharacter.h"

AWeapon::AWeapon()
{
	PrimaryActorTick.bCanEverTick = false;
	
	bReplicates = true;
	
	RootComponent = CreateDefaultSubobject<USceneComponent>("RootComponent");
	
	Mesh = CreateDefaultSubobject<UStaticMeshComponent>("Mesh");
	Mesh->SetupAttachment(RootComponent);
	Mesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	Mesh->SetCollisionResponseToAllChannels(ECR_Ignore);
	Mesh->SetCollisionResponseToChannel(ECC_Camera, ECR_Ignore);
	Mesh->SetIsReplicated(true);
	
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

void AWeapon::ServerHitScan_Implementation()
{
	HitScan();
}

void AWeapon::AttachToCharacter(class ACharacter* NewOwner, FName InSocketName)
{
	WeaponOwner = Cast<ABaseCharacter>(NewOwner);
	if (WeaponOwner && Mesh)
	AttachToComponent(
		WeaponOwner->GetMesh(),
		FAttachmentTransformRules::KeepRelativeTransform,
		InSocketName);
	
	Mesh->SetRelativeLocation(LocationOffset);
	Mesh->SetRelativeRotation(RotationOffset);
	
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
		UE_LOG(LogTemp, Warning, TEXT("Hit count: %d"), HitResults.Num());
		for (FHitResult Hit : HitResults)
		{
			AActor* HitActor = Hit.GetActor();
			if (!HitActor) continue;
			if (HitActors.Contains(HitActor)) continue;
			
			HitActors.AddUnique(HitActor);

			UAbilitySystemComponent* ASC = UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(HitActor);
			
			UE_LOG(LogTemp, Warning, TEXT("Authority: %d (%s)"),
			GetOwner()->HasAuthority(),
			*GetOwner()->GetName());
			if (IsValid(ASC) && IsValid(DamageEffectClass))
			{
				FGameplayEffectContextHandle EffectContext = ASC->MakeEffectContext();
				EffectContext.AddSourceObject(this);

				FGameplayEffectSpecHandle SpecHandle =
					ASC->MakeOutgoingSpec(DamageEffectClass, 1.f, EffectContext);

				if (SpecHandle.IsValid())
				{
					UE_LOG(LogTemp, Warning, TEXT("SpecHandle valid"));
					ASC->ApplyGameplayEffectSpecToSelf(*SpecHandle.Data.Get());
					UE_LOG(LogTemp, Warning, TEXT("Target hit: %s"), *ASC->GetOwner()->GetName());
				}
			}
		}
	}
	
	DrawDebugLine(GetWorld(), Start, End, FColor::Green, false, 2.f, 0, 2.f);
	
	for (auto& Hit : HitResults)
	{
		DrawDebugSphere(GetWorld(), Hit.ImpactPoint, 5.f, 8, FColor::Red, false, 2.f);
	}
}

void AWeapon::HitScanStart(float Interval, TSubclassOf<UGameplayEffect> DamageEffect)
{
	if(bIsHitscanActive) return;
	
	bIsHitscanActive = true;

	DamageEffectClass = DamageEffect;
	
	if (HasAuthority())
	{
		HitActors.Empty();
		
		HitScan();
		GetWorld()->GetTimerManager().SetTimer(
		HitScanTimerHandle,
		this,
		&AWeapon::HitScan,
		Interval,true);
	}
	else
	{
		ServerHitScan_Implementation();
	}
}

void AWeapon::HitScanEnd()
{
	if (!bIsHitscanActive) return;

	bIsHitscanActive = false;
	HitActors.Empty();
	GetWorld()->GetTimerManager().ClearTimer(HitScanTimerHandle);
}

void AWeapon::BeginPlay()
{
	Super::BeginPlay();
	
	WeaponOwner = Cast<ABaseCharacter>(GetOwner());
	if (WeaponOwner && !SocketName.IsNone())
	AttachToCharacter(WeaponOwner, SocketName);
}

