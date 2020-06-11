// Fill out your copyright notice in the Description page of Project Settings.


#include "TPSExplosiveBarrel.h"
#include "HordeGame/Public/Components/TPSHealthComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Components/BoxComponent.h"
#include "Kismet/GameplayStatics.h"
#include "HordeGame/HordeGame.h"
#include "PhysicsEngine/RadialForceComponent.h"

// Sets default values
ATPSExplosiveBarrel::ATPSExplosiveBarrel()
{
	ExplosionRadius = 20.0f;
	ExplosionImpulse = 10.0f;

	HealtComponent = CreateDefaultSubobject<UTPSHealthComponent>(TEXT("Health Component"));
	HealtComponent->OnHealthChanged.AddDynamic(this, &ATPSExplosiveBarrel::OnHealtChanged);

	MeshComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh Component"));
	MeshComp->SetSimulatePhysics(true);
	MeshComp->SetCollisionObjectType(ECC_PhysicsBody);

	RootComponent = MeshComp;

	RadialForceComponent = CreateDefaultSubobject<URadialForceComponent>(TEXT("RadialForce Componet"));
	RadialForceComponent->bImpulseVelChange = true;
	RadialForceComponent->bAutoActivate = false;
	RadialForceComponent->Radius = ExplosionRadius;
	RadialForceComponent->bIgnoreOwningActor = true;
	RadialForceComponent->SetupAttachment(RootComponent);
}

void ATPSExplosiveBarrel::OnHealtChanged(UTPSHealthComponent* OwningHealtComp, float Health, float HealthDelta, const UDamageType* DamageType, AController* InstigatedBy, AActor* DamageCauser)
{
	if (Health <= 0 && !bhasExploded)
	{
		bhasExploded = true;
		UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), ExplosionEffect, GetActorLocation());
		FVector UpwardVector = FVector::UpVector * ExplosionImpulse;
		MeshComp->AddImpulse(UpwardVector,NAME_None,true);
		RadialForceComponent->FireImpulse();

	}
}

