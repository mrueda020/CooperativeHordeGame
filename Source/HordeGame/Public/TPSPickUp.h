// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "TPSPickUp.generated.h"

class USphereComponent;
class UDecalComponent;
class ATPSPowerUp;

UCLASS()
class HORDEGAME_API ATPSPickUp : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ATPSPickUp();

	virtual void NotifyActorBeginOverlap(AActor* OtherActor) override;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY(VisibleAnywhere, Category = "Components")
	USphereComponent* SphereComponent;

	UPROPERTY(VisibleAnywhere, Category = "Components")
	UDecalComponent* DecalComponent;

	UPROPERTY(EditDefaultsOnly, Category = "Power Up")
	TSubclassOf<ATPSPowerUp> PowerUpClass;
	
	UPROPERTY(EditDefaultsOnly, Category = "Power Up")
	float PowerUpDuration;

	FTimerHandle TimerHanldeRespawnPowerUp;

	ATPSPowerUp*  PowerUpInstance;

	void Spawn();
};
