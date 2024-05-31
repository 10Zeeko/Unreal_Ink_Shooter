#include "InkBullets.h"
#include "LevelComponents.h"
#include "Components/ArrowComponent.h"
#include "Components/SphereComponent.h"
#include "Kismet/BlueprintMapLibrary.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
#include "Net/UnrealNetwork.h"
#include "Unreal_Ink_Shooter/Public/Utils.h"

AInkBullets::AInkBullets()
{
	bReplicates = true;
	PrimaryActorTick.bCanEverTick = true;
	mpArrowForward = CreateDefaultSubobject<UArrowComponent>(TEXT("ArrowForward"));
	mpArrowForward->SetupAttachment(RootComponent);

	mpSphereComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("InkBullets"));
	mpSphereComponent->SetupAttachment(mpArrowForward);

	mpCollisionSphere = CreateDefaultSubobject<USphereComponent>("CollisionSphere");
	mpCollisionSphere->SetupAttachment(mpSphereComponent);
	mpCollisionSphere->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Overlap);
	mpCollisionSphere->bHiddenInGame = false;
	mpCollisionSphere->OnComponentBeginOverlap.AddDynamic(this, &AInkBullets::OnOverlapBegin);

	mColorsToCheck = { FColor(255, 0, 0), FColor(0, 0, 255) };
	
	mpProjectileMovementComponent = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileMovement"));
	mpProjectileMovementComponent->bRotationFollowsVelocity = true;
}

void AInkBullets::BeginPlay()
{
	Super::BeginPlay();
	mpDynMaterial = UMaterialInstanceDynamic::Create(mpMaterial, this);
	mpSphereComponent->SetMaterial(0, mpDynMaterial);
	
	GetWorld()->GetTimerManager().SetTimer(MDestroyBulletTimerHandle, this, &AInkBullets::DestroyBullet, 5.0f, false);
}

void AInkBullets::DetectHitInSurface(FTransform aOverlappedActorTransform)
{
	FVector rotation = mpArrowForward->GetForwardVector();
	FCollisionQueryParams traceCollisionParams = GetTraceCollisionParams();

	TArray<FHitResult> bulletHit;
	GetWorld()->LineTraceMultiByProfile(
		bulletHit,
		GetLocation(),
		GetLocation() + rotation * 30.0f,
		TEXT("Ink"),
		traceCollisionParams
	);
	PaintAtPosition(bulletHit);
	Destroy();
}

FRotator AInkBullets::GetLookAtRotation(FTransform aOverlappedActorTransform)
{
	return UKismetMathLibrary::FindLookAtRotation(GetLocation(), aOverlappedActorTransform.GetLocation());
}

FVector AInkBullets::GetLocation()
{
	return mpArrowForward->GetComponentLocation();
}

FCollisionQueryParams AInkBullets::GetTraceCollisionParams()
{
	FCollisionQueryParams traceCollisionParams = FCollisionQueryParams(FName(TEXT("RV_Trace")), true, this);
	traceCollisionParams.bTraceComplex = true;
	traceCollisionParams.bReturnPhysicalMaterial = true;
	traceCollisionParams.bReturnFaceIndex = true;
	return traceCollisionParams;
}

void AInkBullets::PaintAtPosition(TArray<FHitResult>& bulletHit)
{
	if (!HasAuthority()) return;
	
	for (FHitResult& Hit : bulletHit)
	{
		if (Hit.bBlockingHit)
		{
			ALevelComponents* levelComponents = Cast<ALevelComponents>(Hit.GetActor());
			if (levelComponents)
			{
				levelComponents->RPC_Server_PaintAtPosition(this, Hit);
			}
		}
	}
}

void AInkBullets::OnOverlapBegin(UPrimitiveComponent* newComp, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (newComp == mpCollisionSphere)
	{
		ServerOnOverlapBegin(newComp, OtherActor, OtherComp, OtherBodyIndex, bFromSweep, SweepResult);
	}
}

void AInkBullets::ServerOnOverlapBegin_Implementation(UPrimitiveComponent* newComp, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (ALevelComponents* levelComponents = Cast<ALevelComponents>(OtherActor))
	{
		DetectHitInSurface(OtherActor->GetTransform());
	}
	else if (AInkPlayerCharacter* inkPlayerCharacter = Cast<AInkPlayerCharacter>(OtherActor))
	{
		if (inkPlayerCharacter->playerTeam == mpOwnerTeam) return;
		
		UGameplayStatics::ApplyDamage(inkPlayerCharacter, mDamage, nullptr, this, UDamageType::StaticClass());
	}
}

void AInkBullets::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	DOREPLIFETIME(AInkBullets, mpOwnerTeam);
}

void AInkBullets::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	mpDynMaterial->SetVectorParameterValue(TEXT("Direction"), mpSphereComponent->GetForwardVector() * -1.0f);
}

void AInkBullets::DestroyBullet()
{
	Destroy();
}

void AInkBullets::UpdateColor()
{
	mBrushColor = mpOwnerTeam == ETeam::TEAM1 ? FLinearColor(1.0f, 0.0f, 0.0f, 1.0f) : FLinearColor(0.0f, 0.0f, 1.0f, 1.0f);
	mpDynMaterial->SetVectorParameterValue(TEXT("TeamColor"), mBrushColor);
}
