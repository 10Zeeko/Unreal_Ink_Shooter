#include "WeaponChanger.h"
#include "Player/InkPlayerCharacter.h"

AWeaponChanger::AWeaponChanger()
{
	PrimaryActorTick.bCanEverTick = true;
	mpBase = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Base"));
	RootComponent = mpBase;

	mpHitbox = CreateDefaultSubobject<UBoxComponent>(TEXT("Hitbox"));
	mpHitbox->SetupAttachment(RootComponent);
	mpHitbox->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Overlap);
	mpHitbox->OnComponentBeginOverlap.AddDynamic(this, &AWeaponChanger::OnOverlapBegin);
}

void AWeaponChanger::BeginPlay()
{
	Super::BeginPlay();
}

void AWeaponChanger::OnOverlapBegin(UPrimitiveComponent* newComp, AActor* OtherActor, UPrimitiveComponent* OtherComp,
	int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (AInkPlayerCharacter* mPlayerCharacter = Cast<AInkPlayerCharacter>(OtherActor))
	{
		mPlayerCharacter->RPC_Server_PreparePlayerForGame(selectedWeapon);
	}
}

