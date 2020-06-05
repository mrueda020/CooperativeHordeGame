// Fill out your copyright notice in the Description page of Project Settings.

#include "TPSCharacter.h"
#include "Camera/CameraComponent.h"
#include "Components/InputComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "GameFramework/PawnMovementComponent.h"
#include "TPSWeapon.h"
#include "TPSGrenadeLauncher.h"
#include "Engine/World.h"
#include "Components/CapsuleComponent.h"
#include "HordeGame/HordeGame.h"
#include "HordeGame/Public/Components/TPSHealthComponent.h"
#include "Math/UnrealMathUtility.h"
#include <time.h>
// Sets default values
ATPSCharacter::ATPSCharacter()
{
	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	GetMovementComponent()->GetNavAgentPropertiesRef().bCanCrouch = true;

	GetCapsuleComponent()->SetCollisionResponseToChannel(COLLISION_WEAPON, ECR_Ignore);

	HealthComponent = CreateDefaultSubobject<UTPSHealthComponent>(TEXT("HealthComponent"));

	SpringArmComponent = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArmComponent"));
	SpringArmComponent->SetupAttachment(RootComponent);
	//We use to rotate the camera in base of the pawn view rotation
	SpringArmComponent->bUsePawnControlRotation = true;

	CameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("CameraComponent"));
	CameraComponent->SetupAttachment(SpringArmComponent);

	bIsAiming = false;
	ZoomedFOV = 65.0f;
	FOVInterpSpeed = 25.0f;

	WeaponSocketName = "Weapon Socket";
}

// Called when the game starts or when spawned
void ATPSCharacter::BeginPlay()
{
	Super::BeginPlay();

	srand(time(NULL));

	DefaultFOV = CameraComponent->FieldOfView;

	FActorSpawnParameters SpawnParameters;
	SpawnParameters.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
	CurrentWeapon = GetWorld()->SpawnActor<ATPSWeapon>(InitialWeaponClass, FVector::ZeroVector, FRotator::ZeroRotator, SpawnParameters);
	if (CurrentWeapon)
	{
		CurrentWeapon->SetOwner(this);
		CurrentWeapon->AttachToComponent(Cast<USceneComponent>(GetMesh()), FAttachmentTransformRules::SnapToTargetNotIncludingScale, WeaponSocketName);
	}

	HealthComponent->OnHealthChanged.AddDynamic(this, &ATPSCharacter::OnHealtChanged);
}

void ATPSCharacter::OnHealtChanged(UTPSHealthComponent *HealtComp, float Health, float HealthDelta, const UDamageType *DamageType, AController *InstigatedBy, AActor *DamageCauser)
{
	if (Health <= 0 && !bisDeath)
	{
		DeathPoseIndex = rand()% DeathAnimPoses;
		UE_LOG(LogTemp, Warning, TEXT("DeathPose index %d"), DeathPoseIndex);
		bisDeath = true;
		GetMovementComponent()->StopMovementImmediately();
		GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);

		DetachFromControllerPendingDestroy();
		SetLifeSpan(3.0f);
	}
}

// Called every frame
void ATPSCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	float TargetFOV = bIsAiming ? ZoomedFOV : DefaultFOV;

	float NewFOV = FMath::FInterpTo(CameraComponent->FieldOfView, TargetFOV, DeltaTime, FOVInterpSpeed);

	CameraComponent->SetFieldOfView(NewFOV);
}

// Called to bind functionality to input
void ATPSCharacter::SetupPlayerInputComponent(UInputComponent *PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
	PlayerInputComponent->BindAxis("MoveForward", this, &ATPSCharacter::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &ATPSCharacter::MoveRight);

	PlayerInputComponent->BindAxis("LookUp", this, &ATPSCharacter::AddControllerPitchInput);
	PlayerInputComponent->BindAxis("Turn", this, &ATPSCharacter::AddControllerYawInput);

	PlayerInputComponent->BindAction("Crouch", IE_Pressed, this, &ATPSCharacter::BeginCrouch);
	PlayerInputComponent->BindAction("Crouch", IE_Released, this, &ATPSCharacter::EndCrouch);

	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &ATPSCharacter::Jump);
	PlayerInputComponent->BindAction("Jump", IE_Released, this, &ATPSCharacter::StopJumping);

	PlayerInputComponent->BindAction("Zoom", IE_Pressed, this, &ATPSCharacter::BeginZoom);
	PlayerInputComponent->BindAction("Zoom", IE_Released, this, &ATPSCharacter::EndZoom);

	PlayerInputComponent->BindAction("Fire", IE_Pressed, this, &ATPSCharacter::StartFire);
	PlayerInputComponent->BindAction("Fire", IE_Released, this, &ATPSCharacter::EndFire);
}

FVector ATPSCharacter::GetPawnViewLocation() const
{
	if (CameraComponent)
	{
		return CameraComponent->GetComponentLocation();
	}

	return Super::GetPawnViewLocation();
}

void ATPSCharacter::MoveRight(float Value)
{
	if (Value != 0.0f)
	{
		// add movement in that direction
		AddMovementInput(GetActorRightVector(), Value);
	}
}

void ATPSCharacter::MoveForward(float Value)
{
	if (Value != 0.0f)
	{
		// add movement in that direction
		AddMovementInput(GetActorForwardVector(), Value);
	}
}

void ATPSCharacter::BeginCrouch()
{
	Crouch();
}

void ATPSCharacter::EndCrouch()
{
	UnCrouch();
}

void ATPSCharacter::BeginZoom()
{
	bIsAiming = true;
}

void ATPSCharacter::EndZoom()
{
	bIsAiming = false;
}

void ATPSCharacter::StartFire()
{
	if (CurrentWeapon)
	{
		CurrentWeapon->StartFire();
	}
}

void ATPSCharacter::EndFire()
{
	if (CurrentWeapon)
	{
		CurrentWeapon->EndFire();
	}
}
