// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Player/InkPlayerCharacter.h"
#include "InkBullets.generated.h"

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
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	float mDamage {20.0f};
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	ETeam mpOwnerTeam {ETeam::NONE};
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
