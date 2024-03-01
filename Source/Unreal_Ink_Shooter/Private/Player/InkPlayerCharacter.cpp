// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/InkPlayerCharacter.h"

// Sets default values
AInkPlayerCharacter::AInkPlayerCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void AInkPlayerCharacter::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AInkPlayerCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void AInkPlayerCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

