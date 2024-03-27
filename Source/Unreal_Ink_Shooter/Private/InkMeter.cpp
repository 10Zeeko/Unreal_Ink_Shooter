#include "InkMeter.h"

#include "GameFramework/GameModeBase.h"
#include "Kismet/GameplayStatics.h"

AInkMeter::AInkMeter()
{
}

void AInkMeter::CheckInkFromLevelComponents()
{
	// Create an array with the colors of the two teams
	TArray<FColor> TeamsColors;
	TeamsColors.Add(FColor::Red);
	TeamsColors.Add(FColor::Blue);

	// Get the ink values for the two teams
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
	// Send the ink values to the event
	if (apPlayerHud)
	{
		UE_LOG(LogTemp, Warning, TEXT("RedTeamInk: %d, BlueTeamInk: %d"), RedTeamInk, BlueTeamInk);
		
		//apPlayerHud->evOnUpdateInkMeter.Broadcast(RedTeamInk, BlueTeamInk);
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("apPlayerHud is null."));
	}
}

// Called when the game starts or when spawned
void AInkMeter::BeginPlay()
{
	Super::BeginPlay();
	// If apLevelComponents is empty, search and add all ALevelComponents to it
	if (apLevelComponents.Num() == 0)
	{
		TArray<AActor*> mLevelComponents;
		UGameplayStatics::GetAllActorsOfClass(this, ALevelComponents::StaticClass(), mLevelComponents);
		for (AActor* levelComponent : mLevelComponents)
		{
			apLevelComponents.Add(Cast<ALevelComponents>(levelComponent));
		}
	}

	GetWorld()->GetTimerManager().SetTimer(MCheckTimerHandle, this, &AInkMeter::CheckInkFromLevelComponents, 5, true);
}