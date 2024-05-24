#include "TeamChanger.h"
#include "Player/InkPlayerCharacter.h"

class AInkPlayerCharacter;

ATeamChanger::ATeamChanger()
{
	mpBase = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Base"));
	RootComponent = mpBase;

	mpHitbox = CreateDefaultSubobject<UBoxComponent>(TEXT("Hitbox"));
	mpHitbox->SetupAttachment(RootComponent);
	mpHitbox->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Overlap);
	mpHitbox->OnComponentBeginOverlap.AddDynamic(this, &ATeamChanger::OnOverlapBegin);
}

void ATeamChanger::OnOverlapBegin(UPrimitiveComponent* newComp, AActor* OtherActor, UPrimitiveComponent* OtherComp,
	int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (AInkPlayerCharacter* mPlayerCharacter = Cast<AInkPlayerCharacter>(OtherActor))
	{
		mPlayerCharacter->RPC_Server_UpdatePlayerTeam(selectedTeam);
	}
}

void ATeamChanger::BeginPlay()
{
	Super::BeginPlay();
	
}

