#include "InkBullets.h"

#include "LevelComponents.h"
#include "Components/ArrowComponent.h"
#include "Components/SphereComponent.h"
#include "Kismet/BlueprintMapLibrary.h"
#include "Kismet/KismetMathLibrary.h"
#include "Unreal_Ink_Shooter/Public/Utils.h"

AInkBullets::AInkBullets()
{
	apArrowForward = CreateDefaultSubobject<UArrowComponent>(TEXT("ArrowForward"));
	apArrowForward->SetupAttachment(RootComponent);

	apSphereComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("InkBullets"));
	apSphereComponent->SetupAttachment(apArrowForward);

	apCollisionSphere = CreateDefaultSubobject<USphereComponent>("CollisionSphere");
	apCollisionSphere->SetupAttachment(apSphereComponent);
	apCollisionSphere->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Overlap);
	apCollisionSphere->bHiddenInGame = false;
	apCollisionSphere->OnComponentBeginOverlap.AddDynamic(this, &AInkBullets::OnOverlapBegin);

	mColorsToCheck = { FColor(255, 0, 0), FColor(0, 0, 255) };
	
	apProjectileMovementComponent = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileMovement"));
}

void AInkBullets::BeginPlay()
{
	Super::BeginPlay();
}

void AInkBullets::DetectHitInSurface(FTransform aOverlappedActorTransform)
{
	FVector location = apArrowForward->GetComponentLocation();
	FRotator lookAt = UKismetMathLibrary::FindLookAtRotation(location, aOverlappedActorTransform.GetLocation());
	apArrowForward->SetWorldRotation(lookAt);
	FVector rotation = apArrowForward->GetForwardVector();
	FCollisionQueryParams traceCollisionParams = FCollisionQueryParams(FName(TEXT("RV_Trace")), true, this);
	traceCollisionParams.bTraceComplex = true;
	traceCollisionParams.bReturnPhysicalMaterial = true;
	traceCollisionParams.bReturnFaceIndex = true;

	TArray<FHitResult> bulletHit;

	GetWorld()->LineTraceMultiByProfile(
		bulletHit,
		location,
		location + rotation * 30.0f,
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
	Destroy();
}

void AInkBullets::OnOverlapBegin(UPrimitiveComponent* newComp, AActor* OtherActor, UPrimitiveComponent* OtherComp,
	int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (ALevelComponents* levelComponents = Cast<ALevelComponents>(OtherActor))
	{
		DetectHitInSurface(OtherActor->GetTransform());
	}
}

