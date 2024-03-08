#include "InkBullets.h"

#include "LevelComponents.h"
#include "Components/ArrowComponent.h"

AInkBullets::AInkBullets()
{
	PrimaryActorTick.bCanEverTick = true;
	apArrowForward = CreateDefaultSubobject<UArrowComponent>(TEXT("ArrowForward"));
	apArrowForward->SetupAttachment(RootComponent);

	apSphereComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("InkBullets"));
	apSphereComponent->SetupAttachment(apArrowForward);
	
	apProjectileMovementComponent = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileMovement"));
}

void AInkBullets::BeginPlay()
{
	Super::BeginPlay();
	
}

void AInkBullets::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AInkBullets::DetectHitInSurface()
{
	FVector location = apArrowForward->GetComponentLocation();
	FVector rotation = apArrowForward->GetForwardVector();
	FCollisionQueryParams traceCollisionParams = FCollisionQueryParams(FName(TEXT("RV_Trace")), true, this);
	traceCollisionParams.bTraceComplex = true;
	traceCollisionParams.bReturnPhysicalMaterial = true;
	traceCollisionParams.bReturnFaceIndex = true;

	TArray<FHitResult> bulletHit;

	GetWorld()->LineTraceMultiByProfile(
		bulletHit,
		location,
		location + rotation * 15.0f,
		TEXT("Ink"),
		traceCollisionParams
	);

	for (FHitResult& Hit : bulletHit)
	{
		if (Hit.bBlockingHit)
		{
			// Cast to actor LevelComponents and call PaintAtPosition
			ALevelComponents* levelComponents = Cast<ALevelComponents>(Hit.GetActor());

			if (levelComponents)
			{
				levelComponents->PaintAtPosition(this, Hit);
			}
		}
	}
}

