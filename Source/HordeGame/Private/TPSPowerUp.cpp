// Fill out your copyright notice in the Description page of Project Settings.

#include "TPSPowerUp.h"
#include "TimerManager.h"

// Sets default values
ATPSPowerUp::ATPSPowerUp()
{
	PowerUpInverval = 0.0f;
	PowerUpDuration = 0;
	PowerUpProcessedDuration = 0;
}

// Called when the game starts or when spawned
void ATPSPowerUp::BeginPlay()
{
	Super::BeginPlay();
}

void ATPSPowerUp::OnTickPowerUp()
{
	PowerUpProcessedDuration++;
	OnTickedPowerUp();
	if (PowerUpDuration <= PowerUpProcessedDuration)
	{
		OnExpired();

		GetWorldTimerManager().ClearTimer(PowerUpTimeHandle);
	}
}

void ATPSPowerUp::ActivatePowerUp()
{
	OnActivated();
	if (PowerUpInverval > 0.0f)
	{
		GetWorldTimerManager().SetTimer(PowerUpTimeHandle, this, &ATPSPowerUp::OnTickPowerUp, PowerUpInverval, true);
	}
	else
	{
		OnTickPowerUp();
	}
}
