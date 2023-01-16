// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "WeaponBase.h"
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

	void OnFire(USkeletalMeshComponent* SkMesh);

	void OnReload(USkeletalMeshComponent* SkMesh);
};
