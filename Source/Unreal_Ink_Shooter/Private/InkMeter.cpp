#include "InkMeter.h"
#include "GameFramework/GameModeBase.h"
#include "Kismet/GameplayStatics.h"

AInkMeter::AInkMeter()
{
}

void AInkMeter::CheckInkFromLevelComponents()
{
	// Get the ink values for the two teams
	TArray<FColor> TeamsColors = { FColor::Red, FColor::Blue };
	int RedTeamInk = 0;
	int BlueTeamInk = 0;

	for (ALevelComponents* apLevelComponent : apLevelComponents)
	{
		if (ALevelComponents* levelC = Cast<ALevelComponents>(apLevelComponent))
		{
			TArray<int> InkValues = levelC->CheckInk(TeamsColors);
			RedTeamInk += InkValues[0];
			BlueTeamInk += InkValues[1];
		}
	}

	if (apPlayerHud)
	{
		apPlayerHud->evOnUpdateInkMeter.Broadcast(RedTeamInk, BlueTeamInk);
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("apPlayerHud is null."));
	}
}

void AInkMeter::BeginPlay()
{
	Super::BeginPlay();

	if (apLevelComponents.Num() == 0)
	{
		FindAllLevelComponents();
	}

	GetWorld()->GetTimerManager().SetTimer(MCheckTimerHandle, this, &AInkMeter::CheckInkFromLevelComponents, 5, true);
}

void AInkMeter::FindAllLevelComponents()
{
	TArray<AActor*> mLevelComponents;
	UGameplayStatics::GetAllActorsOfClass(this, ALevelComponents::StaticClass(), mLevelComponents);

	for (AActor* levelComponent : mLevelComponents)
	{
		apLevelComponents.Add(Cast<ALevelComponents>(levelComponent));
	}
}
