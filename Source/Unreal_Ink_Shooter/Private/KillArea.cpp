// Fill out your copyright notice in the Description page of Project Settings.


#include "KillArea.h"

#include "Player/InkPlayerCharacter.h"

// Sets default values
AKillArea::AKillArea()
{
	mpBase = CreateDefaultSubobject<UBoxComponent>(TEXT("Base"));
	mpBase->SetupAttachment(RootComponent);
}

// Called when the game starts or when spawned
void AKillArea::BeginPlay()
{
	Super::BeginPlay();
	mpBase->OnComponentBeginOverlap.AddDynamic(this, &AKillArea::OnOverlapBegin);
}

void AKillArea::OnOverlapBegin_Implementation(UPrimitiveComponent* newComp, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (AInkPlayerCharacter* player = Cast<AInkPlayerCharacter>(OtherActor))
	{
		player->RPC_Server_PlayerDies();
	}
}

