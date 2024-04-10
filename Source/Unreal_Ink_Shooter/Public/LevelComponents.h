#pragma once

#include "CoreMinimal.h"
#include "InkBullets.h"
#include "GameFramework/Actor.h"
#include "LevelComponents.generated.h"

UCLASS()
class UNREAL_INK_SHOOTER_API ALevelComponents : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ALevelComponents();

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UStaticMeshComponent* mpStaticMesh;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UTextureRenderTarget2D* mpInkedSurfaceTexture;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UMaterial* mpSurfaceMaterial;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UMaterial* mpBrushMaterial;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<UTexture2D*> mpSplashTextures;

	UPROPERTY()
	TArray<int> mInkValues;

	UPROPERTY()
	UMaterialInstanceDynamic* mpBrushDynMaterial;

	void InitializeResultArray(TArray<FColor> aColorsToCount);
	void CountColors(TArray<FColor>& aColorsToCount, int aSampleSize, TArray<FColor>& aColors);
	void SamplePixels(TArray<FColor>& aColorsToCount);
	UFUNCTION(BlueprintCallable)
	void CheckInk(TArray<FColor>& aColorsToCount);

	void SetBrushSplashAndTexture();
	void SetBrushColor(AInkBullets* aInkBullets);
	void SetBrushParameters(AInkBullets* aInkBullet, FHitResult aHitResult);
	UFUNCTION(BlueprintCallable)
	virtual void PaintAtPosition(AInkBullets* aInkBullet, FHitResult aHitResult);
	bool IsColorMatch(AInkPlayerCharacter* aInkPlayer, FLinearColor aColor);
	UFUNCTION(BlueprintCallable)
	virtual bool CheckInkAtPosition(AInkPlayerCharacter* aInkPlayer, FHitResult aHitResult);
protected:
	void SetUpInkedSurfaceTexture(UMaterialInstanceDynamic* aInkedSurfaceMaterial);
	void SetUpBrushMaterial();
	void SetUpMaterials();
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
};
