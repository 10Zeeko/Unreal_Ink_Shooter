#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "InputActionValue.h"
#include "InkPlayerCharacter.generated.h"

class AWeapon;

UENUM(BlueprintType)
enum class ETeam
{
	NONE = 0 UMETA(DisplayName = "None"),
	TEAM1 UMETA(DisplayName = "Team 1"),
	TEAM2 UMETA(DisplayName = "Team 2"),
	TEAM3 UMETA(DisplayName = "Team 3")
};

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
	UFUNCTION(NetMulticast, Reliable)
	void Shoot(const FInputActionValue& Value);
	UFUNCTION(NetMulticast, Reliable)
	void ResetValues();

	/** Called for swimming **/
	UFUNCTION(Server, Reliable)
	void EnableSwimming();
	UFUNCTION(Server, Reliable)
	void DisableSwimming();
	UFUNCTION(Server, Reliable)
	void checkIfPlayerIsInInk();
	UFUNCTION(Server, Reliable)
	void SwimClimbLineTrace();

	virtual void BeginPlay() override;
private:
	UFUNCTION(Server, Reliable)
	void SetupPlayerWeapon();
	void StopClimbing();
public:
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
};