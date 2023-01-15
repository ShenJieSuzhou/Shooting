// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "WeaponBase.h"
#include "WeaponKnife.generated.h"

/**
 * 
 */
UCLASS()
class SHOOTING_API AWeaponKnife : public AWeaponBase
{
	GENERATED_BODY()
		
public:
	AWeaponKnife();

	void OnFire();

	void OnReload();
};
