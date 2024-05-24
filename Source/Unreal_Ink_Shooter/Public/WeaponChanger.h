// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Weapons/Weapon.h"
#include "Components/BoxComponent.h"
#include "GameFramework/Actor.h"
#include "WeaponChanger.generated.h"

UCLASS()
class UNREAL_INK_SHOOTER_API AWeaponChanger : public AActor
{
	GENERATED_BODY()
	
public:	
	AWeaponChanger();

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	UStaticMeshComponent* mpBase;
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	UBoxComponent* mpHitbox;
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	TSubclassOf<AWeapon> selectedWeapon;

	UFUNCTION()
	virtual void OnOverlapBegin(class UPrimitiveComponent* newComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

protected:
	virtual void BeginPlay() override;

};
