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

	// 最大弹夹量
	MaxAmmoCount = 120;
	// 单个弹夹弹量
	AmmoCount = 30;
	// 子弹精度
	BulletSpread = 0.7;
	// 装弹时间
	ReloadTime = 2;
}


void AWeaponAK::OnFire(USkeletalMeshComponent* SkMesh)
{
	// Load static asset
	FString AkFireMontage = FString(TEXT("AnimMontage'/Game/ShootingPawn/Animations/Arms_AK_ADS_Fire_anim_Montage.Arms_AK_ADS_Fire_anim_Montage'"));
	UAnimMontage* assetMontage = Cast<UAnimMontage>(LoadObject<UAnimMontage>(nullptr, *AkFireMontage));
	FireAnimation = assetMontage;

	//FireAnimation = assetMontage.Object;
	if (FireAnimation != nullptr)
	{
		// Get the animation object for the arms mesh
		UAnimInstance* AnimInstance = SkMesh->GetAnimInstance();
		if (AnimInstance != nullptr)
		{
			AnimInstance->Montage_Play(FireAnimation, 1.f);
		}
	}
	//AmmoCount--;
}

void AWeaponAK::OnReload(USkeletalMeshComponent* SkMesh)
{
	// Play Animation
	// 重新计算子弹数量
	// 如果子弹小于一个弹夹就不换弹
	if(MaxAmmoCount < 30)
	{
		AmmoCount = MaxAmmoCount;
		MaxAmmoCount = 0;
		return;
	}

	MaxAmmoCount = MaxAmmoCount - 30 + AmmoCount;
	AmmoCount = 30;
	

}

bool AWeaponAK::OnCheckAmmo()
{
	if(AmmoCount <= 0)
	{
		return false;
	}

	return true;
}