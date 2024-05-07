#include "Player/InkPlayerState.h"
#include "Unreal_Ink_Shooter/Public/Utils.h"

AInkPlayerState::AInkPlayerState()
{
	PrimaryActorTick.bCanEverTick = true;
}

void AInkPlayerState::PlayerDamaged(float aDamage)
{
	mHP -= aDamage;
}

void AInkPlayerState::PlayerSwimming(float aInkAmount)
{
}

void AInkPlayerState::BeginPlay()
{
	Super::BeginPlay();
	mHP = mMaxHp;
	mPlayerInk = mMaxInk;
	mKills = 0;
	mDeaths = 0;
	mPlayerTeam = ETeam::TEAM1;
}

void AInkPlayerState::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (bIsShooting)
	{
		mPlayerInk -= mWeaponConsumption * DeltaTime;
	}
	else
	{
		mPlayerInk += mInkRegeneration * mInkMultiplier * DeltaTime;
	}
	mPlayerInk = FMath::Clamp(mPlayerInk, 0.0f, mMaxInk);
}