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
	UStaticMeshComponent* apStaticMesh;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UTextureRenderTarget2D* inkedSurfaceTexture;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UMaterial* apSurfaceMaterial;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UMaterial* apBrushMaterial;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<UTexture2D*> apSplashTextures;

	UPROPERTY()
	TArray<int> inkValues;

	UPROPERTY()
	UMaterialInstanceDynamic* brushDynMaterial;

	void InitializeResultArray(TArray<FColor> ColorsToCount);
	void CountColors(TArray<FColor>& Array, int SampleSize, TArray<FColor>& Colors);
	void SamplePixels(TArray<FColor>& Array);
	UFUNCTION(BlueprintCallable)
	void CheckInk(TArray<FColor>& ColorsToCount);

	void SetBrushSplashAndTexture();
	void SetBrushColor(AInkBullets* InkBullets);
	void SetBrushParameters(AInkBullets* aInkBullet, FHitResult aHitResult);
	UFUNCTION(BlueprintCallable)
	virtual void PaintAtPosition(AInkBullets* aInkBullet, FHitResult aHitResult);
	bool IsColorMatch(AInkPlayerCharacter* AInkPlayer, FLinearColor Color);
	UFUNCTION(BlueprintCallable)
	virtual bool CheckInkAtPosition(AInkPlayerCharacter* aInkPlayer, FHitResult aHitResult);
protected:
	void SetUpInkedSurfaceTexture(UMaterialInstanceDynamic* InkedSurfaceMaterial);
	void SetUpBrushMaterial();
	void SetUpMaterials();
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
};
