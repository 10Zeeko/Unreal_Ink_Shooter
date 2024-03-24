#include "InkMeter.h"

#include "Kismet/GameplayStatics.h"

AInkMeter::AInkMeter()
{
}

void AInkMeter::CheckInkFromLevelComponents()
{
	
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
}