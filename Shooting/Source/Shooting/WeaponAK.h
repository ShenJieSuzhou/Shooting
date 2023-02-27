// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "WeaponBase.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Camera/CameraComponent.h"
#include "WeaponAK.generated.h"

class USkeletalMeshComponent;
/**
 * 
 */
UCLASS()
class SHOOTING_API AWeaponAK : public AWeaponBase
{
	GENERATED_BODY()
		
public:
	AWeaponAK();

	virtual void BeginPlay() override;

	void OnFire(USkeletalMeshComponent* SkMesh);

	void OnReload(USkeletalMeshComponent* SkMesh);

	bool OnCheckAmmo();

	void CameraShotLineTrace();

	void GunShotLineTrace(FVector TraceStart, FVector TraceEnd);

	void SpawnBulletDecalTrace(FVector Location, FVector SpawnTransFormLocation, FVector ImpactPoint);

	void SpawnTraceRounder(FVector Location, FVector SpawnTransFormLocation, FVector ImpactPoint);

public:
	FTimerHandle TimerHandle;
};
