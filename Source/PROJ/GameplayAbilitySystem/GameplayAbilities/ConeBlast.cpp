// Fill out your copyright notice in the Description page of Project Settings.


#include "ConeBlast.h"

#include "Engine/OverlapResult.h"
#include "PROJ/Characters/BaseCharacter.h"

UConeBlast::UConeBlast()
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
}

void UConeBlast::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
                                 const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);
	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("UConeBlast Fired!"));
	if (!CommitAbility(Handle,ActorInfo,ActivationInfo))
	{
		EndAbility(Handle,ActorInfo,ActivationInfo,true,false);
	}

	/*Cache player*/
	CachedPlayer = Cast<ABaseCharacter>(GetAvatarActorFromActorInfo());
	if (!CachedPlayer)
	{
		EndAbility(Handle,ActorInfo,ActivationInfo,true,false);
	}
	ExecuteConeAttack();
	EndAbility(Handle,ActorInfo,ActivationInfo,true,false);
}

void UConeBlast::ExecuteConeAttack()
{
	if (!CachedPlayer || !CachedPlayer->HasAuthority()) return;
	/*Get player origin and forward*/
	FVector Origin = CachedPlayer->GetActorLocation();
	FVector ForwardVector = CachedPlayer->GetActorForwardVector();

	TArray<FOverlapResult> Overlaps;
	FCollisionQueryParams Params;
	Params.AddIgnoredActor(CachedPlayer);

	bool bHit = GetWorld()->OverlapMultiByChannel(
		Overlaps,
		Origin,
		FQuat::Identity,
		ECC_Pawn,
		FCollisionShape::MakeSphere(ConeRange),
		Params);

	if (bHit)
	{
		TSet<AActor*> CheckedActors;

		for (const FOverlapResult& Result : Overlaps)
		{
			AActor* HitActor = Result.GetActor();

			if (!HitActor || HitActor == CachedPlayer) continue;
			if (CheckedActors.Contains(HitActor)) continue;
			
			if (IsActorInCone(Origin,ForwardVector,HitActor))
			{
				CheckedActors.Add(HitActor);
				UE_LOG(LogTemp, Warning, TEXT("Cone hit: %s"), *HitActor->GetName());
				ApplyEffectsToTarget(HitActor);
			}
		}
		UE_LOG(LogTemp, Log, TEXT("Cone attack hit %d enemies"), CheckedActors.Num());
	}
	if (bShowDebug)
	{
		DrawConeMesh(Origin,ForwardVector);
	}
}

bool UConeBlast::IsActorInCone(const FVector& Origin, const FVector& ForwardVector, AActor* Target) const
{
	if (!Target) return false;
	
	FVector TargetLocation = Target->GetActorLocation();
	FVector DirectionToTarget = (TargetLocation - Origin).GetSafeNormal();
	/*Calculate distance*/
	float Distance = FVector::Dist(Origin,TargetLocation);
	if (Distance > ConeRange) return false;

	/*Calculate angle between forwardvector and directiontotarget*/
	float DotProduct = FVector::DotProduct(ForwardVector,DirectionToTarget);
	float AngleInRadians = FMath::Acos(DotProduct);
	float AngleInDegrees = FMath::RadiansToDegrees(AngleInRadians);

	/*check cone angle*/
	if (AngleInDegrees > ConeHalfAngle) return false;
	/*Check height*/
	float HeightDifference = FMath::Abs(TargetLocation.Z - Origin.Z);
	if (HeightDifference > ConeHeight) return false;

	return true;
}

void UConeBlast::DrawConeMesh(const FVector& Origin, const FVector& ForwardVector) const
{
	if (!GetWorld()) return;

	// Calculate cone angle in radians
	float HalfAngleRad = FMath::DegreesToRadians(ConeHalfAngle);

	// Get right vector for rotation
	FVector RightVector = FVector::CrossProduct(ForwardVector, FVector::UpVector).GetSafeNormal();
	FVector UpVector = FVector::CrossProduct(RightVector, ForwardVector).GetSafeNormal();

	// Draw cone lines
	for (int32 i = 0; i < DebugLines; ++i)
	{
		float Angle = (360.f / DebugLines) * i;
		float AngleRad = FMath::DegreesToRadians(Angle);

		// Calculate rotation around the forward axis
		FVector Direction = ForwardVector.RotateAngleAxis(ConeHalfAngle, RightVector.RotateAngleAxis(Angle, ForwardVector));
		FVector EndPoint = Origin + Direction * ConeRange;

		// Draw line from origin to cone edge
		DrawDebugLine(GetWorld(), Origin, EndPoint, FColor::Red, false, DebugDuration, 0, 2.f);
	}

	// Draw circles at different distances
	TArray<float> Distances = {ConeRange * 0.33f, ConeRange * 0.66f, ConeRange};
	for (float Distance : Distances)
	{
		float RadiusAtDistance = FMath::Tan(HalfAngleRad) * Distance;
		FVector CircleCenter = Origin + ForwardVector * Distance;
		
		DrawDebugCircle(
			GetWorld(),
			CircleCenter,
			RadiusAtDistance,
			32,
			FColor::Orange,
			false,
			DebugDuration,
			0,
			2.f,
			ForwardVector,
			RightVector,
			false
		);
	}

	// Draw origin sphere
	DrawDebugSphere(GetWorld(), Origin, 20.f, 8, FColor::Green, false, DebugDuration, 0, 2.f);
}
