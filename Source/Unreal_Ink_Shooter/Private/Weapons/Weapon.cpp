#include "Weapons/Weapon.h"
#include "Camera/CameraComponent.h"
#include "Components/ArrowComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/KismetMathLibrary.h"

AWeapon::AWeapon()
{
	mpArrowForward = CreateDefaultSubobject<UArrowComponent>(TEXT("ArrowForward"));
	mpArrowForward->SetupAttachment(RootComponent);

	mpWeaponComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("WeaponMeshComponent"));
	mpWeaponComponent->SetupAttachment(mpArrowForward);
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
	mpWeaponComponent->SetHiddenInGame(true);
	bCanShoot = false;
}

void AWeapon::PlayerShooting()
{
	mpWeaponComponent->SetHiddenInGame(false);
	bCanShoot = true;
}

void AWeapon::Shoot(UCameraComponent& aFollowCamera, UCharacterMovementComponent* aPlayerCharacterMovement)
{
	if (bCanShoot)
	{
		PrepareForShooting(aFollowCamera, aPlayerCharacterMovement);
		GetWorld()->GetTimerManager().SetTimer(mFireRateTimerHandle, this, &AWeapon::FireRateTimer, 1 / mFireRate, false);
	}
}

void AWeapon::PrepareForShooting(UCameraComponent& aFollowCamera, UCharacterMovementComponent* aPlayerCharacterMovement)
{
	FVector CameraForwardVector = aFollowCamera.GetForwardVector();

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

void AWeapon::FireRateTimer()
{
	bCanShoot = true;
}

void AWeapon::BeginPlay()
{
	Super::BeginPlay();
	SetupPlayerWeapon();
}