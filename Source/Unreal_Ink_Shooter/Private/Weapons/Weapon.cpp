#include "Weapons/Weapon.h"
#include "Camera/CameraComponent.h"
#include "Components/ArrowComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "Net/UnrealNetwork.h" // Include this for replication

AWeapon::AWeapon()
{
	mpArrowForward = CreateDefaultSubobject<UArrowComponent>(TEXT("ArrowForward"));
	mpArrowForward->SetupAttachment(RootComponent);

	mpWeaponComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("WeaponMeshComponent"));
	mpWeaponComponent->SetupAttachment(mpArrowForward);
	bReplicates = true;
}

FWeaponsDataRow* AWeapon::GetWeapon(EWeapon aWeapon)
{
	FWeaponsDataRow* StatFound{};

	if (mWeaponsDB)
	{
		FName WeaponString {UEnum::GetDisplayValueAsText(aWeapon).ToString()};
		static const FString FindContext {FString("Searching for ").Append(WeaponString.ToString())};
		StatFound = mWeaponsDB->FindRow<FWeaponsDataRow>(WeaponString, FindContext, true);
	}
	return StatFound;
}

void AWeapon::SetupPlayerWeapon()
{
	const FWeaponsDataRow* aWeapon{GetWeapon(mWeaponSelected)};
	mDamage = aWeapon->BaseDamage;
	mRange = aWeapon->Range;
	mFireRate = aWeapon->FireRate;
	mDispersion = aWeapon->Dispersion;
	mBulletBP = aWeapon->BulletBP;
}

void AWeapon::RPC_Server_PlayerSwimming_Implementation()
{
	RPC_PlayerSwimming();
}

void AWeapon::RPC_PlayerSwimming_Implementation()
{
	mpWeaponComponent->SetHiddenInGame(true);
	bCanShoot = false;
}

void AWeapon::RPC_Server_PrepareForShooting_Implementation(UCameraComponent* aFollowCamera,
	UCharacterMovementComponent* aPlayerCharacterMovement)
{
	RPC_PrepareForShooting(aFollowCamera, aPlayerCharacterMovement);
}

void AWeapon::RPC_PrepareForShooting_Implementation(UCameraComponent* aFollowCamera,
	UCharacterMovementComponent* aPlayerCharacterMovement)
{
	FVector CameraForwardVector = aFollowCamera->GetForwardVector();

	FVector RandomDispersion = FMath::VRand() * mDispersion;
	CameraForwardVector += RandomDispersion;
	CameraForwardVector.Normalize();

	FRotator NewRotation = UKismetMathLibrary::MakeRotFromX(CameraForwardVector);

	FVector SpawnLocation = GetActorLocation();
	mInkBullet = GetWorld()->SpawnActor<AInkBullets>(mBulletBP, SpawnLocation, NewRotation);
	mInkBullet->mpOwnerTeam = mPlayerTeam;
	aPlayerCharacterMovement->RotationRate = FRotator(0.0f, 1800.0f, 00.0f);
	aPlayerCharacterMovement->bOrientRotationToMovement = false;
	aPlayerCharacterMovement->bUseControllerDesiredRotation = true;
	bIsShooting = true;
	bCanShoot = false;
}

void AWeapon::RPC_Server_PlayerShooting_Implementation()
{
	RPC_PlayerShooting();
}
void AWeapon::RPC_PlayerShooting_Implementation()
{
	mpWeaponComponent->SetHiddenInGame(false);
	bCanShoot = true;
}

void AWeapon::RPC_Server_Shoot_Implementation(UCameraComponent* aFollowCamera,
	UCharacterMovementComponent* aPlayerCharacterMovement)
{
	RPC_Shoot(aFollowCamera, aPlayerCharacterMovement);
}

void AWeapon::RPC_Shoot_Implementation(UCameraComponent* aFollowCamera,
	UCharacterMovementComponent* aPlayerCharacterMovement)
{
	if (bCanShoot)
	{
		RPC_Server_PrepareForShooting(aFollowCamera, aPlayerCharacterMovement);
		GetWorld()->GetTimerManager().SetTimer(mFireRateTimerHandle, this, &AWeapon::FireRateTimer, 1 / mFireRate, false);
	}
}

void AWeapon::FireRateTimer()
{
	bCanShoot = true;
}

void AWeapon::BeginPlay()
{
	Super::BeginPlay();
	SetupPlayerWeapon();
}

void AWeapon::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AWeapon, mpWeaponComponent);
	DOREPLIFETIME(AWeapon, mpArrowForward);
	DOREPLIFETIME(AWeapon, mInkBullet);
	DOREPLIFETIME(AWeapon, mPlayerTeam);
}
