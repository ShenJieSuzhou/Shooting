// Fill out your copyright notice in the Description page of Project Settings.


#include "WeaponAK.h"

AWeaponAK::AWeaponAK()
{
    // get knife mesh
	if(FP_Gun)
	{
		auto assetSkeletal = ConstructorHelpers::FObjectFinder<USkeletalMesh>(TEXT("SkeletalMesh'/Game/ShootingPawn/rigged/AK_rigged.AK_rigged'"));
		if(assetSkeletal.Succeeded())
		{
			FP_Gun->SetSkeletalMesh(assetSkeletal.Object);
		}
	}

	// ��󵯼���
	MaxAmmoCount = 120;
	// �������е���
	AmmoCount = 30;
	// �ӵ�����
	BulletSpread = 0.7;
	// װ��ʱ��
	ReloadTime = 2;
}


void AWeaponAK::OnFire(USkeletalMeshComponent* SkMesh)
{
	// Play Montage
}

void AWeaponAK::OnReload(USkeletalMeshComponent* SkMesh)
{
	// Play Animation
}
