// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/InkPlayerCharacter.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "LevelComponents.h"
#include "Components/ArrowComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "Unreal_Ink_Shooter/Public/Utils.h"
#include "Weapons/Weapon.h"

// Sets default values
AInkPlayerCharacter::AInkPlayerCharacter()
{
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	GetCharacterMovement()->bOrientRotationToMovement = true; 

	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->TargetArmLength = 400.0f; 	
	CameraBoom->bUsePawnControlRotation = true;

	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);
	FollowCamera->bUsePawnControlRotation = false;

	apShipMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ShipMesh"));
	apPlayerMesh = GetMesh();
	apShipMesh->SetupAttachment(apPlayerMesh);
	apArrowDown = CreateDefaultSubobject<UArrowComponent>(TEXT("ArrowDown"));
	apArrowDown->SetupAttachment(apPlayerMesh);
	apClimbArrow = CreateDefaultSubobject<UArrowComponent>(TEXT("ClimbArrow"));
	apClimbArrow->SetupAttachment(apPlayerMesh);

	bIsShooting = false;
}

void AInkPlayerCharacter::Move(const FInputActionValue& Value)
{
	FVector2D MovementVector = Value.Get<FVector2D>();

	if (Controller != nullptr)
	{
		// find out which way is forward
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);

		// get forward vector
		FVector MoveDirection;
		if (bIsClimbing)
		{
			MoveDirection = UKismetMathLibrary::GetUpVector(YawRotation);
		}
		else
		{
			MoveDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
		}
		// get right vector 
		const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);

		// add movement 
		AddMovementInput(MoveDirection, MovementVector.Y);
		AddMovementInput(RightDirection, MovementVector.X);
	}
}

void AInkPlayerCharacter::Look(const FInputActionValue& Value)
{
	FVector2D LookAxisVector = Value.Get<FVector2D>();

	if (Controller != nullptr)
	{
		// add yaw and pitch input to controller
		AddControllerYawInput(LookAxisVector.X);
		AddControllerPitchInput(LookAxisVector.Y);
	}
}

void AInkPlayerCharacter::Shoot(const FInputActionValue& Value)
{
	mCurrentWeapon->Shoot(*FollowCamera, GetCharacterMovement());
	bIsShooting = true;
}

void AInkPlayerCharacter::ResetValues()
{
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 360.0f, 0.0f);
	GetCharacterMovement()->bOrientRotationToMovement = true;
	GetCharacterMovement()->bUseControllerDesiredRotation = false;
	bIsShooting = false;
}

void AInkPlayerCharacter::EnableSwimming()
{
	apPlayerMesh->SetHiddenInGame(true);
	mCurrentWeapon->PlayerSwimming();
	apShipMesh->SetHiddenInGame(false);
	playerState = EPlayer::SWIMMING;
	GetWorld()->GetTimerManager().SetTimer(mIsInInkTimerHandle, this, &AInkPlayerCharacter::checkIfPlayerIsInInk, 0.1, true);
}

void AInkPlayerCharacter::DisableSwimming()
{
	apPlayerMesh->SetHiddenInGame(false);
	mCurrentWeapon->PlayerShooting();
	playerState = EPlayer::IDLE;
	apShipMesh->SetHiddenInGame(true);
	GetCharacterMovement()->MaxWalkSpeed = 600.f;
	GetWorld()->GetTimerManager().ClearTimer(mIsInInkTimerHandle);
	GetCharacterMovement()->SetMovementMode(EMovementMode::MOVE_Walking);
	GetCharacterMovement()->bOrientRotationToMovement = true;
	bIsClimbing = false;
}

void AInkPlayerCharacter::checkIfPlayerIsInInk()
{
	if (playerState == EPlayer::SWIMMING)
	{
		FVector location = apArrowDown->GetComponentLocation();
		FVector rotation = apArrowDown->GetForwardVector();
		FCollisionQueryParams traceCollisionParams = FCollisionQueryParams(FName(TEXT("RV_Trace")), true, this);
		traceCollisionParams.bTraceComplex = true;
		traceCollisionParams.bReturnPhysicalMaterial = true;
		traceCollisionParams.bReturnFaceIndex = true;

		TArray<FHitResult> bulletHit;

		GetWorld()->LineTraceMultiByProfile(
			bulletHit,
			location,
			location + rotation * 15.0f,
			TEXT("Ink"),
			traceCollisionParams
		);
		for (FHitResult& Hit : bulletHit)
		{
			if (Hit.bBlockingHit)
			{
				// Cast to actor LevelComponents and call PaintAtPosition
				ALevelComponents* levelComponents = Cast<ALevelComponents>(Hit.GetActor());

				if (levelComponents)
				{
					bIsInInk = levelComponents->CheckInkAtPosition(this, Hit);
					if (bIsInInk)
					{
						GetCharacterMovement()->MaxWalkSpeed = 1200.f;
						// Move the ship under the ink
						apShipMesh->SetRelativeLocation(FVector(0.f, 0.f, -100.f));
					}
					else
					{
						GetCharacterMovement()->MaxWalkSpeed = 300.f;
						apShipMesh->SetRelativeLocation(FVector(0.f, 0.f, 15.f));
					}
				}
				else
				{
					GetCharacterMovement()->MaxWalkSpeed = 300.f;
					apShipMesh->SetRelativeLocation(FVector(0.f, 0.f, 15.f));
				}
			}
		}
	}
	SwimClimbLineTrace();
}

void AInkPlayerCharacter::SwimClimbLineTrace()
{
	FVector start = apClimbArrow->GetComponentLocation();
	FVector end = apClimbArrow->GetForwardVector() * 50.f + start;
	FHitResult hit;
	FCollisionQueryParams traceCollisionParams = FCollisionQueryParams(FName(TEXT("RV_Trace")), true, this);
	traceCollisionParams.bTraceComplex = true;
	traceCollisionParams.bReturnPhysicalMaterial = true;
	traceCollisionParams.bReturnFaceIndex = true;
	GetWorld()->LineTraceSingleByChannel(
			hit,
			start,
			end,
			ECollisionChannel::ECC_Visibility,
			traceCollisionParams
		);
	if (hit.bBlockingHit)
	{
		ALevelComponents* levelComponents = Cast<ALevelComponents>(hit.GetActor());

		if (levelComponents)
		{
			bIsInInk = levelComponents->CheckInkAtPosition(this, hit);
			if (bIsInInk)
			{
				GetCharacterMovement()->SetMovementMode(EMovementMode::MOVE_Flying);
				GetCharacterMovement()->bOrientRotationToMovement = false;
				float rot = UKismetMathLibrary::MakeRotFromX(hit.ImpactNormal).Yaw + 180.0f;
				FRotator newRot = FRotator(GetActorRotation().Pitch, rot, GetActorRotation().Roll);
				SetActorRotation(newRot);
				bIsClimbing = true;
				EMovementMode movementMode = GetCharacterMovement()->GetGroundMovementMode();
				ScreenD(movementMode == EMovementMode::MOVE_Flying ? "Flying" : "Walking");
			}
			else 
			{
				GetCharacterMovement()->SetMovementMode(EMovementMode::MOVE_Falling);
				GetCharacterMovement()->bOrientRotationToMovement = true;
				bIsClimbing = false;
			}
		}
	}
}

// Called when the game starts or when spawned
void AInkPlayerCharacter::BeginPlay()
{
	Super::BeginPlay();
	apShipMesh->SetHiddenInGame(true);
	if (APlayerController* PlayerController = Cast<APlayerController>(Controller))
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
		{
			Subsystem->AddMappingContext(DefaultMappingContext, 0);
		}
	}
	if (IsValid(selectedWeapon))
	{
		SetupPlayerWeapon();
	}
}

void AInkPlayerCharacter::SetupPlayerWeapon()
{
	// Spawn selectedWeapon actor
	FActorSpawnParameters spawnParams;
	spawnParams.Owner = this;
	spawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
	mCurrentWeapon = GetWorld()->SpawnActor<AWeapon>(selectedWeapon, GetActorLocation(), FRotator(0.0f, 0.0f, 0.0f), spawnParams);
	// Attach selectedWeapon actor
	FAttachmentTransformRules attachmentRules(EAttachmentRule::SnapToTarget, EAttachmentRule::KeepRelative, EAttachmentRule::KeepWorld, true);
	mCurrentWeapon->AttachToComponent(GetMesh(), attachmentRules, "WeaponSocket");
}

void AInkPlayerCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

// Called to bind functionality to input
void AInkPlayerCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	if (UEnhancedInputComponent* EnhancedInputComponent = CastChecked<UEnhancedInputComponent>(PlayerInputComponent)) {
		
		//Jumping
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Triggered, this, &ACharacter::Jump);
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Completed, this, &ACharacter::StopJumping);

		//Moving
		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &AInkPlayerCharacter::Move);

		//Looking
		EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &AInkPlayerCharacter::Look);

		//Shooting
		EnhancedInputComponent->BindAction(ShootAction, ETriggerEvent::Triggered, this, &AInkPlayerCharacter::Shoot);
		EnhancedInputComponent->BindAction(ShootAction, ETriggerEvent::Completed, this, &AInkPlayerCharacter::ResetValues);

		//Swimming
		EnhancedInputComponent->BindAction(SwimAction, ETriggerEvent::Started, this, &AInkPlayerCharacter::EnableSwimming);
		EnhancedInputComponent->BindAction(SwimAction, ETriggerEvent::Completed, this, &AInkPlayerCharacter::DisableSwimming);
	}
}