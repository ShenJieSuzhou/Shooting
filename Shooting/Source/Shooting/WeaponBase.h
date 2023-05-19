// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Engine/EngineTypes.h"
#include "Sound/SoundBase.h"
#include "Components/PointLightComponent.h"
#include "Components/StaticMeshComponent.h"
#include "WeaponBase.generated.h"

class USkeletalMeshComponent;
class USceneComponent;
class UAnimMontage;
class USoundBase;

UENUM()
enum class EWeapon : uint8
{	
	EW_None,
	EW_Knife,
	EW_AK,
	EW_AWP,
	EW_M1,
	EW_Pisto
};

UCLASS()
class SHOOTING_API AWeaponBase : public AActor
{
	GENERATED_BODY()
	
public:
	/** Gun mesh: 1st person view (seen only by self) */
	UPROPERTY(VisibleDefaultsOnly, Category = Mesh)
	USkeletalMeshComponent* FP_Gun;

	/** Location on gun mesh where projectiles should spawn. */
	UPROPERTY(VisibleDefaultsOnly, Category = Mesh)
	USceneComponent* FP_MuzzleLocation;
	
	UPROPERTY(VisibleDefaultsOnly, Category = Mesh)
	UPointLightComponent* FP_PointLight;

	UPROPERTY(VisibleDefaultsOnly, Category = Mesh)
	UStaticMeshComponent* FP_FlashPlane;

public:
	FVector GunOffset;

	// Projectile class to spawn
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Gameplay)
	EWeapon WeaponType;

	/** Sound to play each time we fire */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Gameplay)
	USoundBase* FireSound;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Gameplay)
	USoundBase* DryFireSound;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Gameplay)
	USoundBase* ReloadDropSound;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Gameplay)
	USoundBase* ReloadInsertSound;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Gameplay)
	USoundBase* ReloadLoadSound;

	/** AnimMontage to play each time we fire */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Gameplay)
	UAnimMontage* FireAnimation;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Gameplay)
	float ReloadTime;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Gameplay)
	float BulletSpread;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Gameplay)
	int AmmoCount;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Gameplay)
	int MaxAmmoCount;

	int MagazineAmmo;

public:	
	// Sets default values for this actor's properties
	AWeaponBase();

	

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	void SpawnBulletDecalTrace(FHitResult Hit);

	void ApplyDamageTo(FHitResult Hit);
};
