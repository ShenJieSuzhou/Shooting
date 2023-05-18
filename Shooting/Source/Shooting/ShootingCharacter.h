// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "WeaponKnife.h"
#include "WeaponAK.h"
#include "WeaponGlock.h"
#include "WeaponAWP.h"
#include "WeaponBase.h"
#include "PickUpWeaponBase.h"
#include "Kismet/KismetSystemLibrary.h"
#include "ShootingHUD.h"
#include "GunInventory.h"
#include "ShootingCharacter.generated.h"

class UInputComponent;
class USkeletalMeshComponent;
class USceneComponent;
class UCameraComponent;
class UMotionControllerComponent;
class UAnimMontage;
class USoundBase;
class USphereComponent;


UCLASS(config=Game)
class AShootingCharacter : public ACharacter
{
	GENERATED_BODY()
	~AShootingCharacter();
public:
	/** Pawn mesh: 1st person view (arms; seen only by self) */
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category=Mesh)
	USkeletalMeshComponent* Mesh1P;

	/** Gun mesh: 1st person view (seen only by self) */
	UPROPERTY(VisibleDefaultsOnly, Category = Mesh)
	USkeletalMeshComponent* FP_Gun;

	/** Location on gun mesh where projectiles should spawn. */
	UPROPERTY(VisibleDefaultsOnly, Category = Mesh)
	USceneComponent* FP_MuzzleLocation;

	UPROPERTY(VisibleDefaultsOnly, Category = Mesh)
	USceneComponent* DropPoint;

	/** Gun mesh: VR view (attached to the VR controller directly, no arm, just the actual gun) */
	UPROPERTY(VisibleDefaultsOnly, Category = Mesh)
	USkeletalMeshComponent* VR_Gun;

	/** Location on VR gun mesh where projectiles should spawn. */
	UPROPERTY(VisibleDefaultsOnly, Category = Mesh)
	USceneComponent* VR_MuzzleLocation;

	/** First person camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	UCameraComponent* FirstPersonCameraComponent;

	/** Motion controller (right hand) */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	UMotionControllerComponent* R_MotionController;

	/** Motion controller (left hand) */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	UMotionControllerComponent* L_MotionController;

	UPROPERTY(VisibleAnywhere)
	USphereComponent* OverlapCollision;

public:
	AShootingCharacter();

protected:
	virtual void BeginPlay();

public:
	/** Base turn rate, in deg/sec. Other scaling may affect final turn rate. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category=Camera)
	float BaseTurnRate;

	/** Base look up/down rate, in deg/sec. Other scaling may affect final rate. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category=Camera)
	float BaseLookUpRate;

	/** Gun muzzle's offset from the characters location */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=Gameplay)
	FVector GunOffset;

	/** Projectile class to spawn */
	UPROPERTY(EditDefaultsOnly, Category=Projectile)
	TSubclassOf<class AShootingProjectile> ProjectileClass;

	/** Sound to play each time we fire */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=Gameplay)
	USoundBase* FireSound;

	/** AnimMontage to play each time we fire */
	//UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Gameplay)
	UAnimMontage* FireAnimation;

	/** Whether to use motion controller location for aiming. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Gameplay)
	uint8 bUsingMotionControllers : 1;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool WeaponChanged;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int WeaponType;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool IsAimDown;

	bool IsReloading;

	// Weapon Current
	AWeaponBase* CurrentWeapon;

	// Weapon Rifle
	AWeaponAK* WeaponRifle;

	// Weapon Sniper
	AWeaponAWP* WeaponSniper;

	// Weapon Pisto
	AWeaponGlock* WeaponPisto;

	// Weapon Knife
	AWeaponKnife* WeaponKnife;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	EWeapon CurWeaponType;

	FTimerHandle TimerHandle;

	AShootingHUD* hud;

	// 当前碰撞到的武器类型
	EWeapon CurrOverlapWeapon;

	// 当前碰撞到的Actor
	AActor* CollisionActor;

	// 武器背包
	//GunInventory* inventory;

	bool doOnce;

	FTimerHandle AutomaticHandle;

protected:
	
	/** Fires a projectile. */
	void OnFire();

	void OnStopFire();

	void OnReload();

	void OnPickUp();

	void OnDropDown();

	// Weapon1
	void OnHoldRifle();

	// Weapon2
	void OnHoldPisto();

	// Weapon3
	void OnHoldKnife();

	// Weapon4
	void OnHoldSniper();

	UFUNCTION(BlueprintCallable)
	void OnAimDownSight();

	UFUNCTION(BlueprintCallable)
	void OnRecoverAimDownSight();

	UFUNCTION(BlueprintCallable)
	void DropMagazine();

	UFUNCTION(BlueprintCallable)
	void InsertMagazine();

	UFUNCTION(BlueprintCallable)
	void ReloadMagazine();

	UFUNCTION(BlueprintImplementableEvent, Category = "shoot")
	void testSpawnDecal();

	///** Resets HMD orientation and position in VR. */
	//void OnResetVR();

	/** Handles moving forward/backward */
	void MoveForward(float Val);

	/** Handles stafing movement, left and right */
	void MoveRight(float Val);

	/**
	 * Called via input to turn at a given rate.
	 * @param Rate	This is a normalized rate, i.e. 1.0 means 100% of desired turn rate
	 */
	void TurnAtRate(float Rate);

	/**
	 * Called via input to turn look up/down at a given rate.
	 * @param Rate	This is a normalized rate, i.e. 1.0 means 100% of desired turn rate
	 */
	void LookUpAtRate(float Rate);

	struct TouchData
	{
		TouchData() { bIsPressed = false;Location=FVector::ZeroVector;}
		bool bIsPressed;
		ETouchIndex::Type FingerIndex;
		FVector Location;
		bool bMoved;
	};
	void BeginTouch(const ETouchIndex::Type FingerIndex, const FVector Location);
	void EndTouch(const ETouchIndex::Type FingerIndex, const FVector Location);
	void TouchUpdate(const ETouchIndex::Type FingerIndex, const FVector Location);
	TouchData	TouchItem;
	
protected:
	// APawn interface
	virtual void SetupPlayerInputComponent(UInputComponent* InputComponent) override;
	// End of APawn interface

	/* 
	 * Configures input for touchscreen devices if there is a valid touch interface for doing so 
	 *
	 * @param	InputComponent	The input component pointer to bind controls to
	 * @returns true if touch controls were enabled.
	 */
	bool EnableTouchscreenMovement(UInputComponent* InputComponent);
public:
	/** Returns Mesh1P subobject **/
	USkeletalMeshComponent* GetMesh1P() const { return Mesh1P; }
	/** Returns FirstPersonCameraComponent subobject **/
	UCameraComponent* GetFirstPersonCameraComponent() const { return FirstPersonCameraComponent; }

	// Init weapon
	void SetWeapons(EWeapon WeaponT);

	UFUNCTION()
	void OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);

	UFUNCTION() 
	void OnSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OterComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION() 
	void OnSphereEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

};

