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

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Replicated)
	UStaticMeshComponent* mpStaticMesh;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Replicated)
	UTextureRenderTarget2D* mpInkedSurfaceTexture;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Replicated)
	UMaterial* mpSurfaceMaterial;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Replicated)
	UMaterial* mpBrushMaterial;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Replicated)
	TArray<UTexture2D*> mpSplashTextures;

	UPROPERTY(Replicated)
	TArray<int> mInkValues;

	UPROPERTY(Replicated)
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
	virtual void RPC_Server_PaintAtPosition(AInkBullets* aInkBullet, FHitResult aHitResult);
	UFUNCTION(NetMulticast, Reliable)
	void RPC_PaintAtPosition(AInkBullets* aInkBullet, FHitResult aHitResult);
	bool IsColorMatch(AInkPlayerCharacter* aInkPlayer, FLinearColor aColor);
	UFUNCTION(BlueprintCallable)
	virtual bool CheckInkAtPosition(AInkPlayerCharacter* aInkPlayer, FHitResult aHitResult);

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
protected:
	UFUNCTION(Server, Reliable)
	void RPC_Server_SetUpInkedSurfaceTexture(UMaterialInstanceDynamic* aInkedSurfaceMaterial);
	UFUNCTION(NetMulticast, Reliable)
	void RPC_SetUpInkedSurfaceTexture(UMaterialInstanceDynamic* aInkedSurfaceMaterial);
	
	UFUNCTION(Server, Reliable)
	void RPC_Server_SetUpBrushMaterial();
	UFUNCTION(NetMulticast, Reliable)
	void RPC_SetUpBrushMaterial();
	UFUNCTION(Server, Reliable)
	void RPC_Server_SetUpMaterials();
	UFUNCTION(NetMulticast, Reliable)
	void RPC_SetUpMaterials();
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
};
