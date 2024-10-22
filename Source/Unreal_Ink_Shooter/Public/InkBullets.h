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
	void InitializeComponents();

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UStaticMeshComponent* mpSphereComponent;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UProjectileMovementComponent* mpProjectileMovementComponent;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UArrowComponent* mpArrowForward;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UArrowComponent* mpArrowDown;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	USphereComponent* mpCollisionSphere;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<FColor> mColorsToCheck;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	float mPaintSize {1.0f};
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	float mDamage {20.0f};
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Replicated)
	ETeam mpOwnerTeam {ETeam::NONE};

	// Material
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UMaterial* mpMaterial;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UMaterialInstanceDynamic* mpDynMaterial;
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FLinearColor mBrushColor;

	FRotator GetLookAtRotation(const FTransform aTransform);
	FCollisionQueryParams GetTraceCollisionParams();
	FVector GetLocation();

	UPROPERTY()
	FTimerHandle MDestroyBulletTimerHandle;
	
	void PaintAtPosition(TArray<FHitResult>& aHitResultArray);
	UFUNCTION(BlueprintCallable)
	virtual void DetectHitInSurface(FTransform aOverlappedActorTransform);

	UFUNCTION()
	virtual void OnOverlapBegin(class UPrimitiveComponent* newComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	UFUNCTION(Server, Reliable)
	void ServerOnOverlapBegin(class UPrimitiveComponent* newComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	virtual void Tick(float DeltaSeconds) override;
	UFUNCTION()
	void DestroyBullet();

	UFUNCTION()
	void UpdateColor();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
};
