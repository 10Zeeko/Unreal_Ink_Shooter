// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/BoxComponent.h"
#include "GameFramework/Actor.h"
#include "KillArea.generated.h"

UCLASS()
class UNREAL_INK_SHOOTER_API AKillArea : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AKillArea();
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	UBoxComponent* mpBase;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UFUNCTION(Server, Reliable)
	virtual void OnOverlapBegin(class UPrimitiveComponent* newComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
};
