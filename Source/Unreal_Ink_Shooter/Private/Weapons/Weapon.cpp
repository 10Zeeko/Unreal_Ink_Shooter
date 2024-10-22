#include "Weapons/Weapon.h"
#include "Camera/CameraComponent.h"
#include "Components/ArrowComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "Net/UnrealNetwork.h"

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
	mInkConsumption = aWeapon->InkConsumption;
}

void AWeapon::RPC_Server_PlayerSwimming_Implementation()
{
	mpWeaponComponent->SetHiddenInGame(true);
	bCanShoot = false;
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
	FVector CameraForwardVector = aFollowCamera->GetForwardVector();

	FVector RandomDispersion = FMath::VRand() * mDispersion;
	CameraForwardVector += RandomDispersion;
	CameraForwardVector.Normalize();

	FRotator NewRotation = UKismetMathLibrary::MakeRotFromX(CameraForwardVector);

	FActorSpawnParameters SpawnParameters;
	SpawnParameters.Owner = GetOwner();
	SpawnParameters.Instigator = GetInstigator();
	
	FTransform ActorWorldTransform = GetActorTransform();
	FVector WorldSpawnLocation = ActorWorldTransform.TransformPosition(mBulletSpawnPoint);
	
	auto BulletTransform {FTransform(NewRotation, WorldSpawnLocation)};
	mInkBullet = GetWorld()->SpawnActorDeferred<AInkBullets>(mBulletBP, BulletTransform);

	if (mInkBullet != nullptr)
	{
		mInkBullet->mpOwnerTeam = mPlayerTeam;
		aPlayerCharacterMovement->RotationRate = FRotator(0.0f, 1800.0f, 00.0f);
		aPlayerCharacterMovement->bOrientRotationToMovement = false;
		aPlayerCharacterMovement->bUseControllerDesiredRotation = true;
		bIsShooting = true;
		bCanShoot = false;

		mInkBullet->FinishSpawning(BulletTransform);
		mInkBullet->UpdateColor();
	}
	
	RPC_PrepareForShooting(aFollowCamera, aPlayerCharacterMovement);
}

void AWeapon::RPC_PrepareForShooting_Implementation(UCameraComponent* aFollowCamera,
	UCharacterMovementComponent* aPlayerCharacterMovement)
{
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
	DOREPLIFETIME(AWeapon, mBulletSpawnPoint);
	DOREPLIFETIME(AWeapon, mDamage);
	DOREPLIFETIME(AWeapon, mRange);
	DOREPLIFETIME(AWeapon, mFireRate);
	DOREPLIFETIME(AWeapon, mDispersion);
	DOREPLIFETIME(AWeapon, mInkConsumption);
}
