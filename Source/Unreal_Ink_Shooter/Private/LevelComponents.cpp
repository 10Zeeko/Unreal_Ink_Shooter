#include "LevelComponents.h"
#include "Engine/TextureRenderTarget2D.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetRenderingLibrary.h"
#include "Net/UnrealNetwork.h"
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

void ALevelComponents::RPC_Server_CheckInk_Implementation(const TArray<FColor>& aColorsToCount)
{
	if (!mpInkedSurfaceTexture) return;
	mInkValues.Empty();
	RPC_Server_SamplePixels(aColorsToCount);
}

void ALevelComponents::RPC_Server_SamplePixels_Implementation(const TArray<FColor>& ColorsToCount)
{
	int SampleSize = FMath::CeilToInt(mpInkedSurfaceTexture->SizeX * mpInkedSurfaceTexture->SizeY * 0.25f);
	FReadSurfaceDataFlags ReadPixelFlags(RCM_UNorm);
	TArray<FColor> OutBMP;
	mpInkedSurfaceTexture->GameThread_GetRenderTargetResource()->ReadPixels(OutBMP, ReadPixelFlags);
	RPC_Server_CountColors(ColorsToCount, SampleSize, OutBMP);
}

void ALevelComponents::RPC_Server_CountColors_Implementation(const TArray<FColor>& ColorsToCount, int aSampleSize, const TArray<FColor>& OutBMP)
{
	RPC_Server_InitializeResultArray(ColorsToCount);
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

void ALevelComponents::RPC_Server_InitializeResultArray_Implementation(const TArray<FColor>& aColorsToCount)
{
	for (int i = 0; i < aColorsToCount.Num(); ++i)
	{
		mInkValues.Add(0);
	}
}

void ALevelComponents::RPC_Server_PaintAtPosition_Implementation(AInkBullets* aInkBullet, FHitResult aHitResult)
{
	if(!IsValid(mpBrushDynMaterial)) return;
	
	FVector2D UV(0.f, 0.f);
	UGameplayStatics::FindCollisionUV(aHitResult, 0, UV);
	const FLinearColor color = FLinearColor(UV.X, UV.Y, 0.0f, 0.0f);
	float splash = FMath::RandRange(0.0f, 100.0f);
	auto SplashIndex = FMath::RandRange(0, mpSplashTextures.Num() - 1);
	
	RPC_PaintAtPosition(aInkBullet, color, splash, SplashIndex);
}

void ALevelComponents::RPC_PaintAtPosition_Implementation(AInkBullets* aInkBullet, const FLinearColor& aColor, float aSplash, int aSplashTextureIndex)
{
	if(!IsValid(mpBrushDynMaterial)) return;
	if (!IsValid(aInkBullet)) return;
	
	// Set Brush Parameters
	mpBrushDynMaterial->SetVectorParameterValue(TEXT("BrushPosition"), aColor);
	mpBrushDynMaterial->SetScalarParameterValue(TEXT("BrushSize"), aInkBullet->mPaintSize);
	mpBrushDynMaterial->SetScalarParameterValue(TEXT("BrushStrength"), 1.0f);
	
	// Setup Brush Splash and Texture
	mpBrushDynMaterial->SetScalarParameterValue(TEXT("Patron"), aSplash);
	auto SplashTexture = mpSplashTextures[aSplashTextureIndex];
	mpBrushDynMaterial->SetTextureParameterValue(TEXT("SplashTexture"), SplashTexture);
	
	// Set Brush Color
	FLinearColor brushColor = aInkBullet->mpOwnerTeam == ETeam::TEAM1 ? FLinearColor(1.0f, 0.0f, 0.0f, 1.0f) : FLinearColor(0.0f, 0.0f, 1.0f, 1.0f);
	mpBrushDynMaterial->SetVectorParameterValue(TEXT("BrushColor"), brushColor);
	
	UKismetRenderingLibrary::DrawMaterialToRenderTarget(this, mpInkedSurfaceTexture, mpBrushDynMaterial);
}

bool ALevelComponents::CheckInkAtPosition(AInkPlayerCharacter* aInkPlayer, FHitResult aHitResult)
{
	FVector2D UV(0.f, 0.f);
	UGameplayStatics::FindCollisionUV(aHitResult, 0, UV);
	FLinearColor color = UKismetRenderingLibrary::ReadRenderTargetRawUV(this, mpInkedSurfaceTexture, UV.X, UV.Y);
	return IsColorMatch(aInkPlayer, color);
}

void ALevelComponents::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ALevelComponents, mInkValues);
}

void ALevelComponents::RPC_Server_FinalCheckInk_Implementation(const TArray<FColor>& aColorsToCount)
{
	if (!mpInkedSurfaceTexture) return;
	mInkValues.Empty();
	int SampleSize = FMath::CeilToInt(mpInkedSurfaceTexture->SizeX * mpInkedSurfaceTexture->SizeY * 1.0f);
	FReadSurfaceDataFlags ReadPixelFlags(RCM_UNorm);
	TArray<FColor> OutBMP;
	mpInkedSurfaceTexture->GameThread_GetRenderTargetResource()->ReadPixels(OutBMP, ReadPixelFlags);
	RPC_Server_CountColors(aColorsToCount, SampleSize, OutBMP);
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
