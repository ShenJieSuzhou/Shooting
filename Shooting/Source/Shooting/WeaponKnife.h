// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "WeaponBase.h"
#include "WeaponKnife.generated.h"

class USkeletalMeshComponent;
/**
 * 
 */
UCLASS()
class SHOOTING_API AWeaponKnife : public AWeaponBase
{
	GENERATED_BODY()
		
public:
	AWeaponKnife();

	void OnFire(USkeletalMeshComponent* SkMesh);

	void OnReload(USkeletalMeshComponent* SkMesh);
};
