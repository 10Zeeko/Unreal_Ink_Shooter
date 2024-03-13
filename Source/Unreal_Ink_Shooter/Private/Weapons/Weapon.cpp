#include "Weapons/Weapon.h"

#include "Components/ArrowComponent.h"

// Sets default values
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

// Called when the game starts or when spawned
void AWeapon::BeginPlay()
{
	Super::BeginPlay();
	SetupPlayerWeapon();
}

