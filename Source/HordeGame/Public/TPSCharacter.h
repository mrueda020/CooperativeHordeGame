// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "TPSCharacter.generated.h"

class UCameraComponent;
class USpringArmComponent;
class ATPSWeapon;
class UTPSHealthComponent;
UCLASS()
class HORDEGAME_API ATPSCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	ATPSCharacter();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	void MoveRight(float Value);

	void MoveForward(float Value);

	void BeginCrouch();

	void EndCrouch();

	void BeginZoom();

	void EndZoom();

	void StartFire();

	void EndFire();

	ATPSWeapon *CurrentWeapon;

	UPROPERTY(EditDefaultsOnly, Category = "Player")
	TSubclassOf<ATPSWeapon> InitialWeaponClass;

	UPROPERTY(VisibleDefaultsOnly, Category = "Player")
	FName WeaponSocketName;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UCameraComponent *CameraComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	USpringArmComponent *SpringArmComponent;

	bool bIsAiming;

	UPROPERTY(EditDefaultsOnly, Category = "Player")
	float ZoomedFOV;

	float DefaultFOV;

	UPROPERTY(EditDefaultsOnly, Category = "Player", meta = (ClampMin = 0.1, ClampMax = 100.0f))
	float FOVInterpSpeed;

	UTPSHealthComponent *HealthComponent;

	UFUNCTION()
	void OnHealtChanged(UTPSHealthComponent *HealtComp, float Health, float HealthDelta, const class UDamageType *DamageType, class AController *InstigatedBy, AActor *DamageCauser);

	UPROPERTY(BlueprintReadOnly, Category = "Player")
	bool bisDeath;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent *PlayerInputComponent) override;

	virtual FVector GetPawnViewLocation() const override;
};
