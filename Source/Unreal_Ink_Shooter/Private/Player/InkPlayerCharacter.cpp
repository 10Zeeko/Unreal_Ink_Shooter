#include "Player/InkPlayerCharacter.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "LevelComponents.h"
#include "Components/ArrowComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "Net/UnrealNetwork.h"
#include "Unreal_Ink_Shooter/Public/Utils.h"
#include "Weapons/Weapon.h"

AInkPlayerCharacter::AInkPlayerCharacter()
{
	bReplicates = true;
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

void AInkPlayerCharacter::RPC_Server_Shoot_Implementation(const FInputActionValue& Value)
{
	RPC_Shoot(Value);
}

void AInkPlayerCharacter::RPC_Shoot_Implementation(const FInputActionValue& Value)
{
	if(IsLocallyControlled())
	{
		mCurrentWeapon->RPC_Server_Shoot(FollowCamera, GetCharacterMovement());
		bIsShooting = true;
	}
}

void AInkPlayerCharacter::RPC_Server_ResetValues_Implementation()
{
	RPC_ResetValues();
}

void AInkPlayerCharacter::RPC_ResetValues_Implementation()
{
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 360.0f, 0.0f);
	GetCharacterMovement()->bOrientRotationToMovement = true;
	GetCharacterMovement()->bUseControllerDesiredRotation = false;
	bIsShooting = false;
}


void AInkPlayerCharacter::RPC_Server_EnableSwimming_Implementation()
{
	RPC_EnableSwimming();
	playerState = EPlayer::SWIMMING;
	GetWorld()->GetTimerManager().SetTimer(mIsInInkTimerHandle, this, &AInkPlayerCharacter::RPC_Server_checkIfPlayerIsInInk, 0.1, true);
	mpPlayerMesh->SetHiddenInGame(true);
	mpShipMesh->SetHiddenInGame(false);
	mCurrentWeapon->RPC_Server_PlayerSwimming();
}

void AInkPlayerCharacter::RPC_EnableSwimming_Implementation()
{
	mpPlayerMesh->SetHiddenInGame(true);
	mCurrentWeapon->RPC_Server_PlayerSwimming();
	mpShipMesh->SetHiddenInGame(false);
}


void AInkPlayerCharacter::RPC_Server_DisableSwimming_Implementation()
{
	RPC_DisableSwimming();
	playerState = EPlayer::IDLE;
	GetWorld()->GetTimerManager().ClearTimer(mIsInInkTimerHandle);
	GetCharacterMovement()->SetMovementMode(EMovementMode::MOVE_Falling);
	mpShipMesh->SetHiddenInGame(true);
	GetCharacterMovement()->MaxWalkSpeed = 600.f;
	GetCharacterMovement()->bOrientRotationToMovement = true;
	bIsClimbing = false;
}

void AInkPlayerCharacter::RPC_DisableSwimming_Implementation()
{
	mpPlayerMesh->SetHiddenInGame(false);
	mCurrentWeapon->RPC_Server_PlayerShooting();
	playerState = EPlayer::IDLE;
	mpShipMesh->SetHiddenInGame(true);
	GetCharacterMovement()->MaxWalkSpeed = 600.f;
	GetCharacterMovement()->bOrientRotationToMovement = true;
	bIsClimbing = false;
}

void AInkPlayerCharacter::RPC_Server_checkIfPlayerIsInInk_Implementation()
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
		bIsInInk = false;

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
				else
				{
					GetCharacterMovement()->MaxWalkSpeed = 300.f;
					mpShipMesh->SetHiddenInGame(false);
				}
			}
		}
	}
	RPC_checkIfPlayerIsInInk(bIsInInk);
	RPC_Server_SwimClimbLineTrace();
}

void AInkPlayerCharacter::RPC_checkIfPlayerIsInInk_Implementation(bool isInInk)
{
	if (bIsInInk)
	{
		GetCharacterMovement()->MaxWalkSpeed = 1200.f;
		mpShipMesh->SetHiddenInGame(true);
	}
	else
	{
		GetCharacterMovement()->MaxWalkSpeed = 300.f;
		mpShipMesh->SetHiddenInGame(false);
	}
}


void AInkPlayerCharacter::RPC_Server_SwimClimbLineTrace_Implementation()
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
				mNewRot = FRotator(GetActorRotation().Pitch, rot, GetActorRotation().Roll);
				SetActorRotation(mNewRot);
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
		RPC_SwimClimbLineTrace(bIsInInk);
	}
	else
	{
		if (GetCharacterMovement()->MovementMode == EMovementMode::MOVE_Flying)
		{
			GetCharacterMovement()->SetMovementMode(EMovementMode::MOVE_Falling);
			GetCharacterMovement()->bOrientRotationToMovement = true;
			bIsClimbing = false;
			mpShipMesh->SetHiddenInGame(false);
			RPC_SwimClimbLineTrace(false);
		}
	}
}

void AInkPlayerCharacter::RPC_SwimClimbLineTrace_Implementation(bool isInInk)
{
	if (bIsInInk)
	{
		GetCharacterMovement()->SetMovementMode(EMovementMode::MOVE_Flying);
		GetCharacterMovement()->bOrientRotationToMovement = false;
		SetActorRotation(mNewRot);
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
		RPC_Server_SetupPlayerWeapon();
	}
}

void AInkPlayerCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AInkPlayerCharacter, mCurrentWeapon);
	DOREPLIFETIME(AInkPlayerCharacter, mpPlayerMesh);
	DOREPLIFETIME(AInkPlayerCharacter, mpShipMesh);
	DOREPLIFETIME(AInkPlayerCharacter, mpArrowDown);
	DOREPLIFETIME(AInkPlayerCharacter, mpClimbArrow);
	DOREPLIFETIME(AInkPlayerCharacter, InkBullet);
	DOREPLIFETIME(AInkPlayerCharacter, playerTeam);
	DOREPLIFETIME(AInkPlayerCharacter, selectedWeapon);
	DOREPLIFETIME(AInkPlayerCharacter, bIsShooting);
	DOREPLIFETIME(AInkPlayerCharacter, bIsClimbing);
	DOREPLIFETIME(AInkPlayerCharacter, bIsInInk);
	DOREPLIFETIME(AInkPlayerCharacter, mNewRot);
}


void AInkPlayerCharacter::RPC_Server_SetupPlayerWeapon_Implementation()
{
	RPC_SetupPlayerWeapon();
}

void AInkPlayerCharacter::RPC_SetupPlayerWeapon_Implementation()
{
	if (!HasAuthority()) return;
	if (mCurrentWeapon) return;
	FActorSpawnParameters spawnParams;
	spawnParams.Owner = this;
	spawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
	mCurrentWeapon = GetWorld()->SpawnActor<AWeapon>(selectedWeapon, GetActorLocation(), FRotator(0.0f, 0.0f, 0.0f), spawnParams);
	
	if(!IsRunningDedicatedServer())
	{
		FAttachmentTransformRules attachmentRules(EAttachmentRule::SnapToTarget, EAttachmentRule::KeepRelative, EAttachmentRule::KeepWorld, true);
		mCurrentWeapon->AttachToComponent(GetMesh(), attachmentRules, "WeaponSocket");
		mCurrentWeapon->mPlayerTeam = playerTeam;
	}
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
		EnhancedInputComponent->BindAction(ShootAction, ETriggerEvent::Triggered, this, &AInkPlayerCharacter::RPC_Server_Shoot);
		EnhancedInputComponent->BindAction(ShootAction, ETriggerEvent::Completed, this, &AInkPlayerCharacter::RPC_Server_ResetValues);

		//Swimming
		EnhancedInputComponent->BindAction(SwimAction, ETriggerEvent::Started, this, &AInkPlayerCharacter::RPC_Server_EnableSwimming);
		EnhancedInputComponent->BindAction(SwimAction, ETriggerEvent::Completed, this, &AInkPlayerCharacter::RPC_Server_DisableSwimming);
	}
}