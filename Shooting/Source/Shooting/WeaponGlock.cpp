// Fill out your copyright notice in the Description page of Project Settings.


#include "WeaponGlock.h"

AWeaponGlock::AWeaponGlock()
{
    // get knife mesh
	if(FP_Gun)
	{
		auto assetSkeletal = ConstructorHelpers::FObjectFinder<USkeletalMesh>(TEXT("SkeletalMesh'/Game/ShootingPawn/rigged/Glock_Rigged.Glock_Rigged'"));
		if(assetSkeletal.Succeeded())
		{
			FP_Gun->SetSkeletalMesh(assetSkeletal.Object);
		}
	}

	// 最大弹夹量
	MaxAmmoCount = 100;
	// 单个弹夹弹量
	AmmoCount = 20;
	// 子弹精度
	BulletSpread = 0.8;
	// 装弹时间
	ReloadTime = 1.5;
}

void AWeaponGlock::OnFire(USkeletalMeshComponent* SkMesh)
{
	
}

void AWeaponGlock::OnReload(USkeletalMeshComponent* SkMesh)
{

}

bool AWeaponGlock::OnCheckAmmo()
{
	return false;
}


