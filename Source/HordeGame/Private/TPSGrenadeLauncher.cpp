// Fill out your copyright notice in the Description page of Project Settings.


#include "TPSGrenadeLauncher.h"
#include "TPSGrenade.h"
#include "Components/SkeletalMeshComponent.h"
void ATPSGrenadeLauncher::Fire()
{
	auto* MyOwner = GetOwner();
	if (MyOwner&&ProjectileClass)
	{
		FVector EyeLocation;
		FRotator EyeRotator;
		//this would define the eye height location, and view rotation (which is different from the pawn rotation which has a zeroed pitch component)
		MyOwner->GetActorEyesViewPoint(EyeLocation, EyeRotator);
		FActorSpawnParameters ActorSpawnParams;
		ActorSpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
		FVector MuzzleLocation = MeshComp->GetSocketLocation(MuzzleSocketName);
		GetWorld()->SpawnActor<ATPSGrenade>(ProjectileClass, MuzzleLocation, EyeRotator,ActorSpawnParams);	

	}
}