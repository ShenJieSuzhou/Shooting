// Fill out your copyright notice in the Description page of Project Settings.


#include "GunInventory.h"

GunInventory::GunInventory()
{
	WeaponRifle = NULL;
	WeaponPisto = NULL;
	WeaponKnife = NULL;
}

GunInventory::~GunInventory()
{
	
}

void GunInventory::SetRifle(AWeaponAK* rifle)
{
	WeaponRifle = rifle;
}

void GunInventory::SetPisto(AWeaponGlock* pisto)
{
	WeaponPisto = pisto;
}

void GunInventory::SetKnife(AWeaponKnife* knife)
{
	WeaponKnife = knife;
}

bool GunInventory::IsRifleAvaiable()
{
	if(WeaponRifle)
	{
		return true;
	}

	return false;
}

bool GunInventory::IsPistoAvaiable()
{
	if(WeaponPisto)
	{
		return true;
	}

	return false;
}

bool GunInventory::IsKnifeAvaiable()
{
	if(WeaponKnife)
	{
		return true;
	}

	return false;
}

AWeaponAK* GunInventory::GetRifle()
{
	return WeaponRifle;
}

AWeaponGlock* GunInventory::GetPisto()
{
	return WeaponPisto;
}

AWeaponKnife* GunInventory::GetKnife()
{
	return WeaponKnife;
}
