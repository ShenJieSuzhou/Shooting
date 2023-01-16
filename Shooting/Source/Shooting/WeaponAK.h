// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "WeaponBase.h"
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

	void OnFire(USkeletalMeshComponent* SkMesh);

	void OnReload(USkeletalMeshComponent* SkMesh);
};
