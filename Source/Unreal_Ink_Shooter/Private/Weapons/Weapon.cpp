#include "Weapons/Weapon.h"
#include "Camera/CameraComponent.h"
#include "Components/ArrowComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/KismetMathLibrary.h"

AWeapon::AWeapon()
{
	apArrowForward = CreateDefaultSubobject<UArrowComponent>(TEXT("ArrowForward"));
	apArrowForward->SetupAttachment(RootComponent);

	apWeaponComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("WeaponMeshComponent"));
	apWeaponComponent->SetupAttachment(apArrowForward);
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

void AWeapon::PlayerSwimming()
{
	apWeaponComponent->SetHiddenInGame(true);
	bCanShoot = false;
}

void AWeapon::PlayerShooting()
{
	apWeaponComponent->SetHiddenInGame(false);
	bCanShoot = true;
}

void AWeapon::Shoot(UCameraComponent& FollowCamera, UCharacterMovementComponent* playerCharacterMovement)
{
	if (bCanShoot)
	{
		PrepareForShooting(FollowCamera, playerCharacterMovement);
		GetWorld()->GetTimerManager().SetTimer(mFireRateTimerHandle, this, &AWeapon::FireRateTimer, 1 / mFireRate, false);
	}
}

void AWeapon::PrepareForShooting(UCameraComponent& FollowCamera, UCharacterMovementComponent* playerCharacterMovement)
{
	FVector CameraForwardVector = FollowCamera.GetForwardVector();

	FVector RandomDispersion = FMath::VRand() * mDispersion;
	CameraForwardVector += RandomDispersion;
	CameraForwardVector.Normalize();

	FRotator NewRotation = UKismetMathLibrary::MakeRotFromX(CameraForwardVector);

	FVector SpawnLocation = GetActorLocation();
	inkBullet = GetWorld()->SpawnActor<AInkBullets>(mBulletBP, SpawnLocation, NewRotation);
	inkBullet->mpOwnerTeam = playerTeam;
	playerCharacterMovement->RotationRate = FRotator(0.0f, 1800.0f, 00.0f);
	playerCharacterMovement->bOrientRotationToMovement = false;
	playerCharacterMovement->bUseControllerDesiredRotation = true;
	bIsShooting = true;
	bCanShoot = false;
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