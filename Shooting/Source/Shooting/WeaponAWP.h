// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "WeaponBase.h"
#include "Camera/CameraComponent.h"
#include "WeaponAWP.generated.h"


class USkeletalMeshComponent;
/**
 * 
 */
UCLASS()
class SHOOTING_API AWeaponAWP : public AWeaponBase
{
	GENERATED_BODY()
		
public:
	AWeaponAWP();

	void OnFire(USkeletalMeshComponent* SkMesh, bool isAimDown);

	void OnStopFire();

	void OnReload(USkeletalMeshComponent* SkMesh);

	bool OnCheckAmmo();

	void CameraShotLineTrace();

	void GunShotLineTrace(FVector TraceStart, FVector TraceEnd);

	void SpawnBulletDecalTrace(FVector Location, FVector SpawnTransFormLocation, FVector ImpactPoint);

	void SpawnTraceRounder(FVector Location, FVector SpawnTransFormLocation, FVector ImpactPoint);

	void MuzzleFlash();

public:
	UFUNCTION()
	void DelayAndDisplayMuzzle();
};
