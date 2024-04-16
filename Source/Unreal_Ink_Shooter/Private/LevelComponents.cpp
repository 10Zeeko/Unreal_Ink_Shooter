#include "LevelComponents.h"
#include "Engine/TextureRenderTarget2D.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetRenderingLibrary.h"
#include "Unreal_Ink_Shooter/Public/Utils.h"

ALevelComponents::ALevelComponents()
{
	bReplicates = true;
	PrimaryActorTick.bCanEverTick = true;
	mpStaticMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("LevelMesh"));
	mpStaticMesh->SetupAttachment(RootComponent);
}

void ALevelComponents::BeginPlay()
{
	Super::BeginPlay();
	SetUpMaterials();
}

void ALevelComponents::SetUpMaterials()
{
	UMaterialInstanceDynamic* InkedSurfaceMaterial = UMaterialInstanceDynamic::Create(mpSurfaceMaterial, this);
	mpStaticMesh->SetMaterial(0, InkedSurfaceMaterial);
	SetUpInkedSurfaceTexture(InkedSurfaceMaterial);
	SetUpBrushMaterial();
}

void ALevelComponents::SetUpInkedSurfaceTexture(UMaterialInstanceDynamic* aInkedSurfaceMaterial)
{
	mpInkedSurfaceTexture = UKismetRenderingLibrary::CreateRenderTarget2D(this, 512, 512, RTF_RGBA16f);
	aInkedSurfaceMaterial->SetTextureParameterValue(TEXT("InkedSurface"), mpInkedSurfaceTexture);
	aInkedSurfaceMaterial->SetTextureParameterValue(TEXT("NormalTexture"), mpInkedSurfaceTexture);
}

void ALevelComponents::SetUpBrushMaterial()
{
	mpBrushDynMaterial = UMaterialInstanceDynamic::Create(mpBrushMaterial, this);
}

void ALevelComponents::CheckInk(TArray<FColor>& aColorsToCount)
{
	if (!mpInkedSurfaceTexture) return;
	mInkValues.Empty();
	SamplePixels(aColorsToCount);
}

void ALevelComponents::SamplePixels(TArray<FColor>& ColorsToCount)
{
	int SampleSize = FMath::CeilToInt(mpInkedSurfaceTexture->SizeX * mpInkedSurfaceTexture->SizeY * 0.25f);
	FReadSurfaceDataFlags ReadPixelFlags(RCM_UNorm);
	TArray<FColor> OutBMP;
	mpInkedSurfaceTexture->GameThread_GetRenderTargetResource()->ReadPixels(OutBMP, ReadPixelFlags);
	CountColors(ColorsToCount, SampleSize, OutBMP);
}

void ALevelComponents::CountColors(TArray<FColor>& ColorsToCount, int aSampleSize, TArray<FColor>& OutBMP)
{
	InitializeResultArray(ColorsToCount);
	FRandomStream RandStream;
	for (int i = 0; i < aSampleSize; ++i)
	{
		int32 Index = RandStream.RandRange(0, OutBMP.Num() - 1);
		int32 ColorIndex = ColorsToCount.IndexOfByKey(OutBMP[Index]);
		if (ColorIndex != INDEX_NONE)
		{
			mInkValues[ColorIndex]++;
		}
	}
}

void ALevelComponents::InitializeResultArray(TArray<FColor> aColorsToCount)
{
	for (int i = 0; i < aColorsToCount.Num(); ++i)
	{
		mInkValues.Add(0);
	}
}

void ALevelComponents::PaintAtPosition(AInkBullets* aInkBullet, FHitResult aHitResult)
{
	if(!IsValid(mpBrushDynMaterial)) return;
	SetBrushParameters(aInkBullet, aHitResult);
	UKismetRenderingLibrary::DrawMaterialToRenderTarget(this, mpInkedSurfaceTexture, mpBrushDynMaterial);
}

void ALevelComponents::SetBrushParameters(AInkBullets* aInkBullet, FHitResult aHitResult)
{
	FVector2D UV(0.f, 0.f);
	UGameplayStatics::FindCollisionUV(aHitResult, 0, UV);
	const FLinearColor color = FLinearColor(UV.X, UV.Y, 0.0f, 0.0f);
	mpBrushDynMaterial->SetVectorParameterValue(TEXT("BrushPosition"), color);
	mpBrushDynMaterial->SetScalarParameterValue(TEXT("BrushSize"), aInkBullet->mPaintSize);
	mpBrushDynMaterial->SetScalarParameterValue(TEXT("BrushStrength"), 1.0f);
	SetBrushSplashAndTexture();
	SetBrushColor(aInkBullet);
}

void ALevelComponents::SetBrushSplashAndTexture()
{
	float splash = FMath::RandRange(0.0f, 100.0f);
	mpBrushDynMaterial->SetScalarParameterValue(TEXT("Patron"), splash);
	mpBrushDynMaterial->SetTextureParameterValue(TEXT("SplashTexture"), mpSplashTextures[FMath::RandRange(0, mpSplashTextures.Num() - 1)]);
}

void ALevelComponents::SetBrushColor(AInkBullets* aInkBullet)
{
	if (aInkBullet->mpOwnerTeam == ETeam::TEAM1)
	{
		mpBrushDynMaterial->SetVectorParameterValue(TEXT("BrushColor"), FLinearColor(1.0f, 0.0f, 0.0f, 1.0f));
	}
	else
	{
		mpBrushDynMaterial->SetVectorParameterValue(TEXT("BrushColor"), FLinearColor(0.0f, 0.0f, 1.0f, 1.0f));
	}
}

bool ALevelComponents::CheckInkAtPosition(AInkPlayerCharacter* aInkPlayer, FHitResult aHitResult)
{
	FVector2D UV(0.f, 0.f);
	UGameplayStatics::FindCollisionUV(aHitResult, 0, UV);
	FLinearColor color = UKismetRenderingLibrary::ReadRenderTargetRawUV(this, mpInkedSurfaceTexture, UV.X, UV.Y);
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