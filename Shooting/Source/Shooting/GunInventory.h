// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "WeaponGlock.h"
#include "WeaponAK.h"
#include "WeaponKnife.h"

/**
 * 
 */
class SHOOTING_API GunInventory : public UObject
{
public:
	GunInventory();

	~GunInventory();

public:
	void Initialize();

	void SetRifle(AWeaponAK* rifle);

	void SetPisto(AWeaponGlock* pisto);

	void SetKnife(AWeaponKnife* knife);

	bool IsRifleAvaiable();

	bool IsPistoAvaiable();

	bool IsKnifeAvaiable();

	AWeaponAK* GetRifle();
	
	AWeaponGlock* GetPisto();

	AWeaponKnife* GetKnife();

private:
	// Weapon Rifle
	AWeaponAK* WeaponRifle;
	// Weapon Pisto
	AWeaponGlock* WeaponPisto;
	// Weapon Knife
	AWeaponKnife* WeaponKnife;
};
