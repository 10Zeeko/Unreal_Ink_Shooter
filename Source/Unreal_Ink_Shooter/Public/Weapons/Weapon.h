// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "WeaponsDataRow.h"
#include "GameFramework/Actor.h"
#include "Weapon.generated.h"

UCLASS()
class UNREAL_INK_SHOOTER_API AWeapon : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AWeapon();

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UStaticMeshComponent* apWeaponComponent;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UArrowComponent* apArrowForward;
	
#pragma region WEAPON
	FWeaponsDataRow* GetWeapon(EWeapon aWeapon);

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category=mWEAPON)
	EWeapon mWeaponSelected;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category=mWEAPON)
	UDataTable* mWeaponsDB;

	void SetupPlayerWeapon();
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category=mWEAPON)
	TSubclassOf<AInkBullets> mBulletBP {nullptr};
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category=mWEAPON)
	float mDamage {0.0f};
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category=mWEAPON)
	float mRange {0.0f};
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category=mWEAPON)
	float mFireRate {0.0f};
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category=mWEAPON)
	float mDispersion {0.0f};

#pragma endregion
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
};
