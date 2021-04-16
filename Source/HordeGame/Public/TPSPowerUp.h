// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "TPSPowerUp.generated.h"

UCLASS()
class HORDEGAME_API ATPSPowerUp : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ATPSPowerUp();

	UFUNCTION(BlueprintImplementableEvent, Category = "PowerUp")
	void OnActivated();

	UFUNCTION(BlueprintImplementableEvent, Category = "PowerUp")
	void OnExpired();

	UFUNCTION(BlueprintImplementableEvent, Category = "PowerUp")
	void OnTickedPowerUp();

	void ActivatePowerUp();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	
	//Time between tick
	UPROPERTY(EditDefaultsOnly, Category = "PowerUp")
	float PowerUpInverval;

	//Total time we applied the powerup
	UPROPERTY(EditDefaultsOnly, Category = "PowerUp")
	int32 PowerUpDuration;
	
	//Total number of ticks processed
	int32 PowerUpProcessedDuration;

	FTimerHandle PowerUpTimeHandle;

	UFUNCTION()
	void OnTickPowerUp();


};
