// Fill out your copyright notice in the Description page of Project Settings.


#include "WeaponAK.h"
#include "ShootingHUD.h"
#include "Kismet/GameplayStatics.h"

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

	// 最大携带弹量
	MaxAmmoCount = 120;
	// 当前弹量
	AmmoCount = 30;
	// 弹夹装载量
	MagazineAmmo = 30;
	// 子弹精度
	BulletSpread = 0.7;
	// 装弹时间
	ReloadTime = 2.0;
}

void AWeaponAK::BeginPlay()
{
	Super::BeginPlay();
	
	//AShootingHUD* hud = Cast<AShootingHUD>(UGameplayStatics::GetPlayerController(this, 0)->GetHUD());
	//hud->UpdateAmmo(AmmoCount, MagazineAmmo, MaxAmmoCount);
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

	AmmoCount--;
	AShootingHUD* hud = Cast<AShootingHUD>(UGameplayStatics::GetPlayerController(this, 0)->GetHUD());
	hud->UpdateAmmo(AmmoCount, MagazineAmmo, MaxAmmoCount);
}

void AWeaponAK::OnReload(USkeletalMeshComponent* SkMesh)
{
	// 重新计算子弹数量
	// 如果子弹小于一个弹夹就不换弹
	if(MaxAmmoCount < 30)
	{
		AmmoCount = MaxAmmoCount;
		MaxAmmoCount = 0;
		MagazineAmmo = 0;
		return;
	}

	MaxAmmoCount = MaxAmmoCount - 30 + AmmoCount;

	// Load static asset
	FString AkReloadAnimation = FString(TEXT("AnimSequence'/Game/ShootingPawn/Animations/AK_Reload_anim.AK_Reload_anim'"));
	UAnimationAsset* assetAnim = Cast<UAnimationAsset>(LoadObject<UAnimationAsset>(nullptr, *AkReloadAnimation));

	if (assetAnim != nullptr)
	{
		FP_Gun->PlayAnimation(assetAnim, false);
	}

	AmmoCount = MagazineAmmo;
	AShootingHUD* hud = Cast<AShootingHUD>(UGameplayStatics::GetPlayerController(this, 0)->GetHUD());
	hud->UpdateAmmo(AmmoCount, MagazineAmmo, MaxAmmoCount);
}

bool AWeaponAK::OnCheckAmmo()
{
	if(AmmoCount <= 0)
	{
		return false;
	}

	return true;
}