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

	// ��󵯼���
	MaxAmmoCount = 100;
	// �������е���
	AmmoCount = 20;
	// �ӵ�����
	BulletSpread = 0.8;
	// װ��ʱ��
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


