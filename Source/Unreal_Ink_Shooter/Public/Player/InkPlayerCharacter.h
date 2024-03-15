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

	// Ink Bullet blueprint
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Bullet")
	TSubclassOf<class AInkBullets> InkBullet;

	// Team of the player
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	ETeam playerTeam {ETeam::NONE};
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	TSubclassOf<AWeapon> selectedWeapon;
	UPROPERTY()
	AWeapon* mCurrentWeapon;
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
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
	void Shoot(const FInputActionValue& Value);
	void ResetValues();

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

private:
	void SetupPlayerWeapon();
};
