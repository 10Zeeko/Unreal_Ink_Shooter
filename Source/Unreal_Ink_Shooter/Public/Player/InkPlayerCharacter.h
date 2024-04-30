#pragma once

#include "CoreMinimal.h"
#include "InkPlayerState.h"
#include "GameFramework/Character.h"
#include "InputActionValue.h"
#include "InkPlayerCharacter.generated.h"

class AWeapon;

UENUM()
enum class EPlayer
{
	IDLE UMETA(DisplayName = "Idle"),
	SHOOTING UMETA(DisplayName = "Shooting"),
	SWIMMING UMETA(DisplayName = "Swimming")
};

UCLASS()
class UNREAL_INK_SHOOTER_API AInkPlayerCharacter : public ACharacter
{
	GENERATED_BODY()

public:

	AInkPlayerCharacter();
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class USpringArmComponent* CameraBoom;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class UCameraComponent* FollowCamera;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	class UInputMappingContext* DefaultMappingContext;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	class UInputAction* JumpAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	class UInputAction* MoveAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	class UInputAction* LookAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	class UInputAction* ShootAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	class UInputAction* SwimAction;
	
	UPROPERTY(Replicated) 
	USkeletalMeshComponent* mpPlayerMesh;

	// Swim
	FTimerHandle mIsInInkTimerHandle;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Replicated)
	UStaticMeshComponent* mpShipMesh;
	EPlayer playerState {EPlayer::IDLE};
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Replicated)
	UArrowComponent* mpArrowDown;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Replicated)
	UArrowComponent* mpClimbArrow;
	UPROPERTY(Replicated)
	FRotator mNewRot;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Replicated)
	bool bIsInInk {false};
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Replicated)
	bool bIsClimbing {false};

	// Ink Bullet blueprint
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Bullet", Replicated)
	TSubclassOf<class AInkBullets> InkBullet;

	// Team of the player
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Replicated)
	ETeam playerTeam {ETeam::NONE};
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Replicated)
	TSubclassOf<AWeapon> selectedWeapon;
	UPROPERTY(Replicated)
	AWeapon* mCurrentWeapon;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Replicated)
	bool bIsShooting;

	UPROPERTY()
	AInkPlayerState* mPlayerState;
	FTimerHandle mGetPlayerStateHandle;

	void GetPlayerState();

	virtual void Tick(float DeltaTime) override;
	
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	FORCEINLINE class USpringArmComponent* GetCameraBoom() const { return CameraBoom; }

	FORCEINLINE class UCameraComponent* GetFollowCamera() const { return FollowCamera; }
protected:
	/** Called for movement input */
	void Move(const FInputActionValue& Value);

	/** Called for looking input */
	void Look(const FInputActionValue& Value);

	/** Called for shooting input */
	UFUNCTION(Server, Reliable)
	void RPC_Server_Shoot(const FInputActionValue& Value);
	UFUNCTION(NetMulticast, Reliable)
	void RPC_Shoot(const FInputActionValue& Value);
	
	UFUNCTION(Server, Reliable)
	void RPC_Server_ResetValues();
	UFUNCTION(NetMulticast, Reliable)
	void RPC_ResetValues();

	/** Called for swimming **/
	UFUNCTION(Server, Reliable)
	void RPC_Server_EnableSwimming();
	UFUNCTION(NetMulticast, Reliable)
	void RPC_EnableSwimming();
	
	UFUNCTION(Server, Reliable)
	void RPC_Server_DisableSwimming();
	UFUNCTION(NetMulticast, Reliable)
	void RPC_DisableSwimming();
	
	UFUNCTION(Server, Reliable)
	void RPC_Server_checkIfPlayerIsInInk();
	UFUNCTION(NetMulticast, Reliable)
	void RPC_checkIfPlayerIsInInk(bool isInInk);
	
	UFUNCTION(Server, Reliable)
	void RPC_Server_SwimClimbLineTrace();
	UFUNCTION(NetMulticast, Reliable)
	void RPC_SwimClimbLineTrace(bool isInInk);

	// Update player team
	UFUNCTION(Server, Reliable)
	void RPC_Server_UpdatePlayerTeam(ETeam aNewTeam);
	UFUNCTION(NetMulticast, Reliable)
	void RPC_UpdatePlayerTeam(ETeam aNewTeam);

	virtual void BeginPlay() override;

	virtual float TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser) override;
private:
	UFUNCTION(Server, Reliable)
	void RPC_Server_SetupPlayerWeapon();
	UFUNCTION(NetMulticast, Reliable)
	void RPC_SetupPlayerWeapon();
public:
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
};