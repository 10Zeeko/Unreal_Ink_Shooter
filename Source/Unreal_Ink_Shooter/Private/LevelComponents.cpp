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

	inkedSurfaceTexture = UKismetRenderingLibrary::CreateRenderTarget2D(this, 2048, 2048, RTF_RGBA16f);
	InkedSurfaceMaterial->SetTextureParameterValue(TEXT("InkedSurface"), inkedSurfaceTexture);

	brushDynMaterial = UMaterialInstanceDynamic::Create(apBrushMaterial, this);
}

void ALevelComponents::PaintAtPosition(AInkBullets* aInkBullet, FHitResult aHitResult)
{
	FVector2D UV(0.f, 0.f);
	// Find Collision UV
	UGameplayStatics::FindCollisionUV(aHitResult, 0, UV);
	FLinearColor color = FLinearColor(UV.X, UV.Y, 0.0f, 0.0f);
	
	UKismetMaterialLibrary::SetVectorParameterValue(GetWorld(), Cast<UMaterialParameterCollection>(brushDynMaterial), TEXT("BrushPosition"), color);
	UKismetMaterialLibrary::SetScalarParameterValue(GetWorld(), Cast<UMaterialParameterCollection>(brushDynMaterial), TEXT("BrushSize"), aInkBullet->mPaintSize);
	UKismetMaterialLibrary::SetScalarParameterValue(GetWorld(), Cast<UMaterialParameterCollection>(brushDynMaterial), TEXT("BrushStrength"), 1.0f);
	
	if (aInkBullet->mpOwnerTeam == ETeam::TEAM1)
	{
		UKismetMaterialLibrary::SetVectorParameterValue(GetWorld(), Cast<UMaterialParameterCollection>(brushDynMaterial), TEXT("BrushColor"), FLinearColor(1.0f, 0.0f, 0.0f, 1.0f));
	}
	else
	{
		UKismetMaterialLibrary::SetVectorParameterValue(GetWorld(), Cast<UMaterialParameterCollection>(brushDynMaterial), TEXT("BrushColor"), FLinearColor(0.0f, 0.0f, 1.0f, 1.0f));
	}
	UKismetRenderingLibrary::DrawMaterialToRenderTarget(GetWorld(), inkedSurfaceTexture, brushDynMaterial);
}

// Called every frame
void ALevelComponents::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

