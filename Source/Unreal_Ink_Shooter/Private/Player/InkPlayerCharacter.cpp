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

	mpShipMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ShipMesh"));
	mpPlayerMesh = GetMesh();
	mpShipMesh->SetupAttachment(mpPlayerMesh);
	mpArrowDown = CreateDefaultSubobject<UArrowComponent>(TEXT("ArrowDown"));
	mpArrowDown->SetupAttachment(mpPlayerMesh);
	mpClimbArrow = CreateDefaultSubobject<UArrowComponent>(TEXT("ClimbArrow"));
	mpClimbArrow->SetupAttachment(mpPlayerMesh);

	bIsShooting = false;
}

void AInkPlayerCharacter::Move(const FInputActionValue& Value)
{
	FVector2D MovementVector = Value.Get<FVector2D>();

	if (Controller != nullptr)
	{
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);

		FVector MoveDirection = bIsClimbing ? UKismetMathLibrary::GetUpVector(YawRotation) : FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
		const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);

		AddMovementInput(MoveDirection, MovementVector.Y);
		AddMovementInput(RightDirection, MovementVector.X);
	}
}

void AInkPlayerCharacter::Look(const FInputActionValue& Value)
{
	FVector2D LookAxisVector = Value.Get<FVector2D>();

	if (Controller != nullptr)
	{
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
	mpPlayerMesh->SetHiddenInGame(true);
	mCurrentWeapon->PlayerSwimming();
	mpShipMesh->SetHiddenInGame(false);
	playerState = EPlayer::SWIMMING;
	GetWorld()->GetTimerManager().SetTimer(mIsInInkTimerHandle, this, &AInkPlayerCharacter::checkIfPlayerIsInInk, 0.1, true);
}

void AInkPlayerCharacter::DisableSwimming()
{
	mpPlayerMesh->SetHiddenInGame(false);
	mCurrentWeapon->PlayerShooting();
	playerState = EPlayer::IDLE;
	mpShipMesh->SetHiddenInGame(true);
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
		FVector location = mpArrowDown->GetComponentLocation();
		FVector rotation = mpArrowDown->GetForwardVector();
		FCollisionQueryParams traceCollisionParams = FCollisionQueryParams(FName(TEXT("RV_Trace")), true, this);
		traceCollisionParams.bTraceComplex = true;
		traceCollisionParams.bReturnPhysicalMaterial = true;
		traceCollisionParams.bReturnFaceIndex = true;

		FHitResult checkSurfaceHit;

		GetWorld()->LineTraceSingleByChannel(
			checkSurfaceHit,
			location,
			location + rotation * 15.0f,
			ECollisionChannel::ECC_Visibility,
			traceCollisionParams
		);
		GetCharacterMovement()->MaxWalkSpeed = 300.f;
		mpShipMesh->SetHiddenInGame(false);
		
		if (checkSurfaceHit.bBlockingHit)
		{
			ALevelComponents* levelComponents = Cast<ALevelComponents>(checkSurfaceHit.GetActor());
			if (levelComponents)
			{
				bIsInInk = levelComponents->CheckInkAtPosition(this, checkSurfaceHit);
				
				if (bIsInInk)
				{
					GetCharacterMovement()->MaxWalkSpeed = 1200.f;
					mpShipMesh->SetHiddenInGame(true);
				}
			}
		}
	}
	SwimClimbLineTrace();
}

void AInkPlayerCharacter::SwimClimbLineTrace()
{
	FVector start = mpClimbArrow->GetComponentLocation();
	FVector end = mpClimbArrow->GetForwardVector() * 50.f + start;
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
				mpShipMesh->SetHiddenInGame(true);
			}
			else 
			{
				GetCharacterMovement()->SetMovementMode(EMovementMode::MOVE_Falling);
				GetCharacterMovement()->bOrientRotationToMovement = true;
				bIsClimbing = false;
				mpShipMesh->SetHiddenInGame(false);
			}
		}
	}
	else
	{
		if (GetCharacterMovement()->MovementMode == EMovementMode::MOVE_Flying)
		{
			GetCharacterMovement()->SetMovementMode(EMovementMode::MOVE_Falling);
			GetCharacterMovement()->bOrientRotationToMovement = true;
			bIsClimbing = false;
			mpShipMesh->SetHiddenInGame(false);
		}
	}
}

void AInkPlayerCharacter::BeginPlay()
{
	Super::BeginPlay();
	mpShipMesh->SetHiddenInGame(true);
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
	FActorSpawnParameters spawnParams;
	spawnParams.Owner = this;
	spawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
	mCurrentWeapon = GetWorld()->SpawnActor<AWeapon>(selectedWeapon, GetActorLocation(), FRotator(0.0f, 0.0f, 0.0f), spawnParams);
	FAttachmentTransformRules attachmentRules(EAttachmentRule::SnapToTarget, EAttachmentRule::KeepRelative, EAttachmentRule::KeepWorld, true);
	mCurrentWeapon->AttachToComponent(GetMesh(), attachmentRules, "WeaponSocket");
}

void AInkPlayerCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

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