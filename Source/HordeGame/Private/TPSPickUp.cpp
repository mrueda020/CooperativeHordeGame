// Fill out your copyright notice in the Description page of Project Settings.

#include "TPSPickUp.h"
#include "TPSPowerUp.h"
#include "Components/SphereComponent.h"
#include "Components/DecalComponent.h"
#include "TimerManager.h"

// Sets default values
ATPSPickUp::ATPSPickUp()
{
	SphereComponent = CreateDefaultSubobject<USphereComponent>(TEXT("SphereComponent"));
	SphereComponent->SetSphereRadius(65.0f);
	RootComponent = SphereComponent;

	DecalComponent = CreateDefaultSubobject<UDecalComponent>(TEXT("DecalComponent"));
	DecalComponent->SetRelativeRotation(FRotator(90.0f, 0.0f, 0.0f));
	DecalComponent->DecalSize = FVector(65, 65, 65);
	DecalComponent->SetupAttachment(RootComponent);
}

void ATPSPickUp::NotifyActorBeginOverlap(AActor *OtherActor)
{
	Super::NotifyActorBeginOverlap(OtherActor);
	UE_LOG(LogTemp, Warning, TEXT("Power up  to spawn"))
	if (PowerUpInstance)
	{
		PowerUpInstance->ActivatePowerUp();
		PowerUpInstance = nullptr;
		GetWorldTimerManager().SetTimer(TimerHanldeRespawnPowerUp, this, &ATPSPickUp::Spawn, PowerUpDuration);
	}
}

// Called when the game starts or when spawned
void ATPSPickUp::BeginPlay()
{
	Super::BeginPlay();
	Spawn();
}

void ATPSPickUp::Spawn()
{
	if (PowerUpClass == nullptr)
	{

		return;
	}
	FActorSpawnParameters SpawnParameters;
	SpawnParameters.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
	PowerUpInstance = GetWorld()->SpawnActor<ATPSPowerUp>(PowerUpClass, GetTransform(), SpawnParameters);
}
