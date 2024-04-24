#include "Player/InkPlayerState.h"

void AInkPlayerState::PlayerDamaged(float aDamage)
{
	mHP -= aDamage;
}

void AInkPlayerState::BeginPlay()
{
	Super::BeginPlay();
	mHP = mMaxHp;
	mPlayerInk = mMaxInk;
	mKills = 0;
	mDeaths = 0;
}
