#pragma once

#include "CoreMinimal.h"
#include "WeaponsDataRow.h"
#include "GameFramework/Actor.h"
#include "Player/InkPlayerCharacter.h"
#include "Weapon.generated.h"

UCLASS()
class UNREAL_INK_SHOOTER_API AWeapon : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AWeapon();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Replicated) 
	UStaticMeshComponent* mpWeaponComponent;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Replicated) 
	UArrowComponent* mpArrowForward;

	UPROPERTY(Replicated) 
	AInkBullets* mInkBullet {};

#pragma region WEAPON
	FWeaponsDataRow* GetWeapon(EWeapon aWeapon);

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category=mWEAPON, Replicated) 
	EWeapon mWeaponSelected;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category=mWEAPON, Replicated) 
	UDataTable* mWeaponsDB;

	void SetupPlayerWeapon();
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category=mWEAPON, Replicated) 
	TSubclassOf<AInkBullets> mBulletBP {nullptr};
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category=mWEAPON, Replicated) 
	float mDamage {0.0f};
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category=mWEAPON, Replicated) 
	float mRange {0.0f};
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category=mWEAPON, Replicated) 
	float mFireRate {0.0f};
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category=mWEAPON, Replicated) 
	float mDispersion {0.0f};

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Replicated) 
	ETeam mPlayerTeam {ETeam::NONE};
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Replicated) 
	bool bIsShooting;
	UPROPERTY(Replicated)
	bool bCanShoot {true};

	FTimerHandle mFireRateTimerHandle;
#pragma endregion

	void PlayerSwimming();
	UFUNCTION(NetMulticast, Reliable)
	void PlayerShooting();

	UFUNCTION(Server, Reliable)
	void PrepareForShooting(UCameraComponent* aFollowCamera, UCharacterMovementComponent* aPlayerCharacterMovement);
	UFUNCTION(Server, Reliable)
	void Shoot(UCameraComponent* aFollowCamera, UCharacterMovementComponent* aPlayerCharacterMovement);
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	// Override this function to replicate properties
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
private:
	void FireRateTimer();
};
