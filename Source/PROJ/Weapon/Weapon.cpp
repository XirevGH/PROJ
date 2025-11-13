

#include "Weapon.h"
#include "Components/CapsuleComponent.h"
#include "PROJ/BaseCharacter.h"

AWeapon::AWeapon()
{
	PrimaryActorTick.bCanEverTick = false;
	
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
	ECC_Visibility, // Replace with your collision channel
	TraceParams);
	
	if (bHit)
	{
		for (auto& Hit : HitResults)
		{
			if (AActor* HitActor = Hit.GetActor())
			UE_LOG(LogTemp, Warning, TEXT("Hit Actor: %s"), *HitActor->GetName());
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

