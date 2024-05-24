// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/BoxComponent.h"
#include "Player/InkPlayerState.h"
#include "TeamChanger.generated.h"

UCLASS()
class UNREAL_INK_SHOOTER_API ATeamChanger : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ATeamChanger();
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	UStaticMeshComponent* mpBase;
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	UBoxComponent* mpHitbox;
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	ETeam selectedTeam {ETeam::NONE};

	UFUNCTION()
	virtual void OnOverlapBegin(class UPrimitiveComponent* newComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
};
