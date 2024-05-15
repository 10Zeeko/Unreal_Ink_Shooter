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

	UPROPERTY(Replicated)
	TArray<int> mInkValues;

	UPROPERTY()
	UMaterialInstanceDynamic* mpBrushDynMaterial;

	UFUNCTION(Server, Reliable)
	void RPC_Server_InitializeResultArray(const TArray<FColor>& aColorsToCount);
	UFUNCTION(Server, Reliable)
	void RPC_Server_CountColors(const TArray<FColor>& aColorsToCount, int aSampleSize, const TArray<FColor>& aColors);
	UFUNCTION(Server, Reliable)
	void RPC_Server_SamplePixels(const TArray<FColor>& aColorsToCount);
	UFUNCTION(BlueprintCallable, Server, Reliable)
	void RPC_Server_CheckInk(const TArray<FColor>& aColorsToCount);

	UFUNCTION(BlueprintCallable, Server, Reliable)
	void RPC_Server_FinalCheckInk(const TArray<FColor>& aColorsToCount);
	
	UFUNCTION(BlueprintCallable, Server, Reliable)
	virtual void RPC_Server_PaintAtPosition(AInkBullets* aInkBullet, FHitResult aHitResult);
	UFUNCTION(NetMulticast, Reliable)
	void RPC_PaintAtPosition(AInkBullets* aInkBullet, const FLinearColor& aColor, float aSplash, int aSplashTextureIndex);
	bool IsColorMatch(AInkPlayerCharacter* aInkPlayer, FLinearColor aColor);
	
	
	UFUNCTION(BlueprintCallable)
	virtual bool CheckInkAtPosition(AInkPlayerCharacter* aInkPlayer, FHitResult aHitResult);

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
protected:
	void SetUpInkedSurfaceTexture(UMaterialInstanceDynamic* aInkedSurfaceMaterial);
	
	void SetUpBrushMaterial();
	void SetUpMaterials();
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
};
