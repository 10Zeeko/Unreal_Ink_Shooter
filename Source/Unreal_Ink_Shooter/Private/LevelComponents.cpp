#include "LevelComponents.h"
#include "Engine/TextureRenderTarget2D.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetRenderingLibrary.h"
#include "Unreal_Ink_Shooter/Public/Utils.h"

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

	inkedSurfaceTexture = UKismetRenderingLibrary::CreateRenderTarget2D(this, 512, 512, RTF_RGBA16f);
	InkedSurfaceMaterial->SetTextureParameterValue(TEXT("InkedSurface"), inkedSurfaceTexture);
	InkedSurfaceMaterial->SetTextureParameterValue(TEXT("NormalTexture"), inkedSurfaceTexture);

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

	// Select random float number for splash and store it in a variable
	float splash = FMath::RandRange(0.0f, 100.0f);
	brushDynMaterial->SetScalarParameterValue(TEXT("Patron"), splash);
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

bool ALevelComponents::CheckInkAtPosition(AInkPlayerCharacter* aInkPlayer, FHitResult aHitResult)
{
	FVector2D UV(0.f, 0.f);
	// Find Collision UV
	UGameplayStatics::FindCollisionUV(aHitResult, 0, UV);
	FLinearColor color = UKismetRenderingLibrary::ReadRenderTargetRawUV(this, inkedSurfaceTexture, UV.X, UV.Y);
	
	// Debug string to check what color is there
	// ScreenD(color.ToString());
	switch (aInkPlayer->playerTeam)
	{
		case ETeam::TEAM1:
			if (color.R >= 0.8f)
			{
				return true;
			}
			break;
		case ETeam::TEAM2:
			if (color.B >= 0.8f)
			{
				return true;
			}
			break;
		default:
			break;
	}
	return false;
}

// Called every frame
void ALevelComponents::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

