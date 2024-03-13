#include "LevelComponents.h"
#include "Engine/TextureRenderTarget2D.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMaterialLibrary.h"
#include "Kismet/KismetRenderingLibrary.h"
#include "Materials/MaterialParameterCollection.h"

// Sets default values
ALevelComponents::ALevelComponents()
{
	PrimaryActorTick.bCanEverTick = true;

	apStaticMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("LevelMesh"));
	apStaticMesh->SetupAttachment(RootComponent);
}

// Called when the game starts or when spawned
void ALevelComponents::BeginPlay()
{
	Super::BeginPlay();
	UMaterialInstanceDynamic* InkedSurfaceMaterial = UMaterialInstanceDynamic::Create(apSurfaceMaterial, this);
	
	apStaticMesh->SetMaterial(0, InkedSurfaceMaterial);

	inkedSurfaceTexture = UKismetRenderingLibrary::CreateRenderTarget2D(this, 8192, 8192, RTF_RGBA16f);
	InkedSurfaceMaterial->SetTextureParameterValue(TEXT("InkedSurface"), inkedSurfaceTexture);

	brushDynMaterial = UMaterialInstanceDynamic::Create(apBrushMaterial, this);
}

void ALevelComponents::PaintAtPosition(AInkBullets* aInkBullet, FHitResult aHitResult)
{
	if(!IsValid(brushDynMaterial)) return;
	
	FVector2D UV(0.f, 0.f);
	// Find Collision UV
	UGameplayStatics::FindCollisionUV(aHitResult, 0, UV);
	const FLinearColor color = FLinearColor(UV.X, UV.Y, 0.0f, 0.0f);

	
	brushDynMaterial->SetVectorParameterValue(TEXT("BrushPosition"), color);
	brushDynMaterial->SetScalarParameterValue(TEXT("BrushSize"), aInkBullet->mPaintSize);
	brushDynMaterial->SetScalarParameterValue(TEXT("BrushStrength"), 1.0f);

	// Select random texture for SplashTexture parameter in material
	brushDynMaterial->SetTextureParameterValue(TEXT("SplashTexture"), apSplashTextures[FMath::RandRange(0, apSplashTextures.Num() - 1)]);
	
	if (aInkBullet->mpOwnerTeam == ETeam::TEAM1)
	{
		brushDynMaterial->SetVectorParameterValue(TEXT("BrushColor"), FLinearColor(1.0f, 0.0f, 0.0f, 1.0f));
	}
	else
	{
		brushDynMaterial->SetVectorParameterValue(TEXT("BrushColor"), FLinearColor(0.0f, 0.0f, 1.0f, 1.0f));
	}
	UKismetRenderingLibrary::DrawMaterialToRenderTarget(this, inkedSurfaceTexture, brushDynMaterial);
}

// Called every frame
void ALevelComponents::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

