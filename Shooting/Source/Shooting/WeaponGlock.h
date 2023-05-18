// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "WeaponBase.h"
#include "Camera/CameraComponent.h"
#include "ShootingProjectile.h"
#include "WeaponGlock.generated.h"


class USkeletalMeshComponent;
/**
 * 
 */
UCLASS()
class SHOOTING_API AWeaponGlock : public AWeaponBase
{
	GENERATED_BODY()
		
public:
	AWeaponGlock();

	void OnFire(USkeletalMeshComponent* SkMesh, bool isAimDown);

	void OnStopFire();

	void OnReload(USkeletalMeshComponent* SkMesh);

	bool OnCheckAmmo();

	void CameraShotLineTrace();

	void GunShotLineTrace(FVector TraceStart, FVector TraceEnd);

	void MuzzleFlash();

public:
	UFUNCTION()
	void DelayAndDisplayMuzzle();
};
