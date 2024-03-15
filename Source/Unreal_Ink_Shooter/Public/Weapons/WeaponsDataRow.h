#pragma once
#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "InkBullets.h"
#include "WeaponsDataRow.generated.h"

UENUM(BlueprintType)
enum class EWeapon : uint8
{
	NONE = 0 UMETA(DisplayName = "None"),
	GUN UMETA(DisplayName = "Gun"),
	DISPERSERGUN UMETA(DisplayName = "Disperser Gun"),
	SHOTGUN UMETA(DisplayName = "Shotgun"),
	SNIPER UMETA(DisplayName = "Sniper")
};

USTRUCT(Blueprintable, BlueprintType)
struct FWeaponsDataRow : public FTableRowBase
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	EWeapon Name {EWeapon::NONE};
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString Description;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TWeakObjectPtr<UTexture2D> Image {nullptr};
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSubclassOf<AInkBullets> BulletBP {nullptr};
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float BaseDamage;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float Range;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float FireRate;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float Dispersion;
};
