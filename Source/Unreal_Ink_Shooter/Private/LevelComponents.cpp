#include "LevelComponents.h"
#include "Engine/TextureRenderTarget2D.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetRenderingLibrary.h"
#include "Unreal_Ink_Shooter/Public/Utils.h"

ALevelComponents::ALevelComponents()
{
	PrimaryActorTick.bCanEverTick = true;
	apStaticMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("LevelMesh"));
	apStaticMesh->SetupAttachment(RootComponent);
}

void ALevelComponents::BeginPlay()
{
	Super::BeginPlay();
	SetUpMaterials();
}

void ALevelComponents::SetUpMaterials()
{
	UMaterialInstanceDynamic* InkedSurfaceMaterial = UMaterialInstanceDynamic::Create(apSurfaceMaterial, this);
	apStaticMesh->SetMaterial(0, InkedSurfaceMaterial);
	SetUpInkedSurfaceTexture(InkedSurfaceMaterial);
	SetUpBrushMaterial();
}

void ALevelComponents::SetUpInkedSurfaceTexture(UMaterialInstanceDynamic* InkedSurfaceMaterial)
{
	inkedSurfaceTexture = UKismetRenderingLibrary::CreateRenderTarget2D(this, 512, 512, RTF_RGBA16f);
	InkedSurfaceMaterial->SetTextureParameterValue(TEXT("InkedSurface"), inkedSurfaceTexture);
	InkedSurfaceMaterial->SetTextureParameterValue(TEXT("NormalTexture"), inkedSurfaceTexture);
}

void ALevelComponents::SetUpBrushMaterial()
{
	brushDynMaterial = UMaterialInstanceDynamic::Create(apBrushMaterial, this);
}

void ALevelComponents::CheckInk(TArray<FColor>& ColorsToCount)
{
	if (!inkedSurfaceTexture) return;
	inkValues.Empty();
	SamplePixels(ColorsToCount);
}

void ALevelComponents::SamplePixels(TArray<FColor>& ColorsToCount)
{
	int SampleSize = FMath::CeilToInt(inkedSurfaceTexture->SizeX * inkedSurfaceTexture->SizeY * 0.25f);
	FReadSurfaceDataFlags ReadPixelFlags(RCM_UNorm);
	TArray<FColor> OutBMP;
	inkedSurfaceTexture->GameThread_GetRenderTargetResource()->ReadPixels(OutBMP, ReadPixelFlags);
	CountColors(ColorsToCount, SampleSize, OutBMP);
}

void ALevelComponents::CountColors(TArray<FColor>& ColorsToCount, int SampleSize, TArray<FColor>& OutBMP)
{
	InitializeResultArray(ColorsToCount);
	FRandomStream RandStream;
	for (int i = 0; i < SampleSize; ++i)
	{
		int32 Index = RandStream.RandRange(0, OutBMP.Num() - 1);
		int32 ColorIndex = ColorsToCount.IndexOfByKey(OutBMP[Index]);
		if (ColorIndex != INDEX_NONE)
		{
			inkValues[ColorIndex]++;
		}
	}
}

void ALevelComponents::InitializeResultArray(TArray<FColor> ColorsToCount)
{
	for (int i = 0; i < ColorsToCount.Num(); ++i)
	{
		inkValues.Add(0);
	}
}

void ALevelComponents::PaintAtPosition(AInkBullets* aInkBullet, FHitResult aHitResult)
{
	if(!IsValid(brushDynMaterial)) return;
	SetBrushParameters(aInkBullet, aHitResult);
	UKismetRenderingLibrary::DrawMaterialToRenderTarget(this, inkedSurfaceTexture, brushDynMaterial);
}

void ALevelComponents::SetBrushParameters(AInkBullets* aInkBullet, FHitResult aHitResult)
{
	FVector2D UV(0.f, 0.f);
	UGameplayStatics::FindCollisionUV(aHitResult, 0, UV);
	const FLinearColor color = FLinearColor(UV.X, UV.Y, 0.0f, 0.0f);
	brushDynMaterial->SetVectorParameterValue(TEXT("BrushPosition"), color);
	brushDynMaterial->SetScalarParameterValue(TEXT("BrushSize"), aInkBullet->mPaintSize);
	brushDynMaterial->SetScalarParameterValue(TEXT("BrushStrength"), 1.0f);
	SetBrushSplashAndTexture();
	SetBrushColor(aInkBullet);
}

void ALevelComponents::SetBrushSplashAndTexture()
{
	float splash = FMath::RandRange(0.0f, 100.0f);
	brushDynMaterial->SetScalarParameterValue(TEXT("Patron"), splash);
	brushDynMaterial->SetTextureParameterValue(TEXT("SplashTexture"), apSplashTextures[FMath::RandRange(0, apSplashTextures.Num() - 1)]);
}

void ALevelComponents::SetBrushColor(AInkBullets* aInkBullet)
{
	if (aInkBullet->mpOwnerTeam == ETeam::TEAM1)
	{
		brushDynMaterial->SetVectorParameterValue(TEXT("BrushColor"), FLinearColor(1.0f, 0.0f, 0.0f, 1.0f));
	}
	else
	{
		brushDynMaterial->SetVectorParameterValue(TEXT("BrushColor"), FLinearColor(0.0f, 0.0f, 1.0f, 1.0f));
	}
}

bool ALevelComponents::CheckInkAtPosition(AInkPlayerCharacter* aInkPlayer, FHitResult aHitResult)
{
	FVector2D UV(0.f, 0.f);
	UGameplayStatics::FindCollisionUV(aHitResult, 0, UV);
	FLinearColor color = UKismetRenderingLibrary::ReadRenderTargetRawUV(this, inkedSurfaceTexture, UV.X, UV.Y);
	return IsColorMatch(aInkPlayer, color);
}

bool ALevelComponents::IsColorMatch(AInkPlayerCharacter* aInkPlayer, FLinearColor color)
{
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