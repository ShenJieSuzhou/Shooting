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
	ReloadTime = 0.2;
}


void AWeaponAK::OnFire(USkeletalMeshComponent* SkMesh)
{
	// Load static asset
	FString AkFireAnimation = FString(TEXT("AnimSequence'/Game/ShootingPawn/Animations/Arms_AK_Fire_anim.Arms_AK_Fire_anim'"));
	UAnimationAsset* assetAnim = Cast<UAnimationAsset>(LoadObject<UAnimationAsset>(nullptr, *AkFireAnimation));

	if (assetAnim != nullptr)
	{
		SkMesh->PlayAnimation(assetAnim, false);
	}
	//AmmoCount--;
}

void AWeaponAK::OnReload(USkeletalMeshComponent* SkMesh)
{
	// ���¼����ӵ�����
	// ����ӵ�С��һ�����оͲ�����
	if(MaxAmmoCount < 30)
	{
		AmmoCount = MaxAmmoCount;
		MaxAmmoCount = 0;
		return;
	}

	MaxAmmoCount = MaxAmmoCount - 30 + AmmoCount;
	AmmoCount = 30;

	// Play Animation

	// Load static asset
	FString AkReloadAnimation = FString(TEXT("AnimSequence'/Game/ShootingPawn/Animations/AK_Reload_anim.AK_Reload_anim'"));
	UAnimationAsset* assetAnim = Cast<UAnimationAsset>(LoadObject<UAnimationAsset>(nullptr, *AkReloadAnimation));

	if (assetAnim != nullptr)
	{
		SkMesh->PlayAnimation(assetAnim, false);
	}

	/*FTimerHandle TimerHandle;
	GetWorld()->GetTimerManager().SetTimer(TimerHandle, this, &UObject::MethodWithDelay, 3, false);*/
}

bool AWeaponAK::OnCheckAmmo()
{
	if(AmmoCount <= 0)
	{
		return false;
	}

	return true;
}