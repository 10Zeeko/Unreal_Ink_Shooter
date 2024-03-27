// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Player/InkPlayerCharacter.h"
#include "InkBullets.generated.h"

class USphereComponent;
class AWeapon;

UCLASS()
class UNREAL_INK_SHOOTER_API AInkBullets : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AInkBullets();

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UStaticMeshComponent* apSphereComponent;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UProjectileMovementComponent* apProjectileMovementComponent;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UArrowComponent* apArrowForward;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UArrowComponent* apArrowDown;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	USphereComponent* apCollisionSphere;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<FColor> mColorsToCheck;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	float mPaintSize {0.1f};
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	float mDamage {20.0f};
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	ETeam mpOwnerTeam {ETeam::NONE};

	UFUNCTION(BlueprintCallable)
	virtual void DetectHitInSurface();

	UFUNCTION()
	virtual void OnOverlapBegin(class UPrimitiveComponent* newComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
};
