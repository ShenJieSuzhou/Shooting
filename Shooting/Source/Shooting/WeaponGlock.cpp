// Fill out your copyright notice in the Description page of Project Settings.


#include "WeaponGlock.h"

#include "DrawDebugHelpers.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
#include "Math/UnrealMathUtility.h"
#include "ShootingHUD.h"
#include "ShootingCharacter.h"

AWeaponGlock::AWeaponGlock()
{
    // get knife mesh
	if(FP_Gun)
	{
		auto assetSkeletal = ConstructorHelpers::FObjectFinder<USkeletalMesh>(TEXT("SkeletalMesh'/Game/ShootingPawn/rigged/Glock_Rigged.Glock_Rigged'"));
		if(assetSkeletal.Succeeded())
		{
			FP_Gun->SetSkeletalMesh(assetSkeletal.Object);
			FP_MuzzleLocation->SetWorldLocation(FVector(17.0f, 0, 7.0f));
			FP_PointLight->SetWorldLocation(FVector(39.0f, -5.0f, 5.f));

			FString GunMuzzle = FString(TEXT("StaticMesh'/Game/ShootingPawn/Meshs/FlashPlane.plane_ground_3x3'"));
			UStaticMesh* mesh = Cast<UStaticMesh>(LoadObject<UStaticMesh>(nullptr, *GunMuzzle));
			FP_FlashPlane->SetStaticMesh(mesh);
			FP_FlashPlane->SetCollisionEnabled(ECollisionEnabled::NoCollision);
			FP_FlashPlane->SetWorldRotation(FRotator(90.f, 90.f, 90.f));
			FP_FlashPlane->SetWorldLocation(FVector(22.f, 1, 7.37f));
			FP_FlashPlane->SetWorldScale3D(FVector(0.1f, 0.1f, 0.1f));
		}
	}

	// 最大弹夹量
	MaxAmmoCount = 100;
	// 单个弹夹弹量
	AmmoCount = 20;
	// 弹夹装载量
	MagazineAmmo = 20;
	// 子弹精度
	BulletSpread = 150.f;
	// 装弹时间
	ReloadTime = 1.5;
}

void AWeaponGlock::OnFire(USkeletalMeshComponent* SkMesh, bool isAimDown)
{
	// 播放声音
	FString GunShotSound = FString(TEXT("SoundWave'/Game/ShootingPawn/Sounds/Gunshot.Gunshot'"));
	FireSound = Cast<USoundBase>(LoadObject<USoundBase>(nullptr, *GunShotSound));
	if (FireSound != nullptr)
	{
		UGameplayStatics::PlaySoundAtLocation(this, FireSound, GetActorLocation());
	}

	if(!isAimDown)
	{
		// Load static asset
		FString GlockFireMontage = FString(TEXT("AnimMontage'/Game/ShootingPawn/Animations/Arms_Glock_Fire_anim_Montage.Arms_Glock_Fire_anim_Montage'"));
		UAnimMontage* assetMontage = Cast<UAnimMontage>(LoadObject<UAnimMontage>(nullptr, *GlockFireMontage));
		FireAnimation = assetMontage;

		if (FireAnimation != nullptr)
		{
			// Get the animation object for the arms mesh
			UAnimInstance* AnimInstance = SkMesh->GetAnimInstance();
			if (AnimInstance != nullptr)
			{
				AnimInstance->Montage_Play(FireAnimation, 0.8f);
			}
		}
	}
	else
	{
		// Load static asset
		FString GlockFireMontage = FString(TEXT("AnimMontage'/Game/ShootingPawn/Animations/Arms_Glock_ADS_Fire_anim_Montage.Arms_Glock_ADS_Fire_anim_Montage'"));
		UAnimMontage* assetMontage = Cast<UAnimMontage>(LoadObject<UAnimMontage>(nullptr, *GlockFireMontage));
		FireAnimation = assetMontage;

		if (FireAnimation != nullptr)
		{
			// Get the animation object for the arms mesh
			UAnimInstance* AnimInstance = SkMesh->GetAnimInstance();
			if (AnimInstance != nullptr)
			{
				AnimInstance->Montage_Play(FireAnimation, 0.8f);
			}
		}
	}
	
	// Load static asset
	FString GlockFireAnimation = FString(TEXT("AnimSequence'/Game/ShootingPawn/Animations/Glock_Fire_anim.Glock_Fire_anim'"));
	UAnimationAsset* assetAnim = Cast<UAnimationAsset>(LoadObject<UAnimationAsset>(nullptr, *GlockFireAnimation));
	if (assetAnim != nullptr)
	{
		FP_Gun->PlayAnimation(assetAnim, false);
	}

	//发射子弹
	AmmoCount--;
	AShootingHUD* hud = Cast<AShootingHUD>(UGameplayStatics::GetPlayerController(this, 0)->GetHUD());
	hud->UpdateAmmo(AmmoCount, MagazineAmmo, MaxAmmoCount);

	// Camera Shot
	CameraShotLineTrace();
}

void AWeaponGlock::OnStopFire()
{

}


void AWeaponGlock::CameraShotLineTrace()
{
	AShootingCharacter* MyPawn = Cast<AShootingCharacter>(UGameplayStatics::GetPlayerCharacter(GetWorld(), 0));
	UCameraComponent* FirstCamera = MyPawn->FirstPersonCameraComponent;
	FVector TraceStart = FirstCamera->GetComponentLocation();

	float calu = BulletSpread * -1;
	float x = FMath::RandRange(calu, BulletSpread);
	float y = FMath::RandRange(calu, BulletSpread);
	float z = FMath::RandRange(calu, BulletSpread);
	FVector TraceEnd = TraceStart + FirstCamera->GetForwardVector() * 9000.f + FVector(x, y, z);

	FHitResult Hit;
	FCollisionQueryParams queryParam;
	queryParam.AddIgnoredActor(this);
	queryParam.bReturnPhysicalMaterial = true;
	bool isHit = GetWorld()->LineTraceSingleByChannel(Hit, TraceStart, TraceEnd, ECC_Visibility, queryParam);
	
	MuzzleFlash();

	if (isHit)
	{
		SpawnBulletDecalTrace(Hit);
	}
}

void AWeaponGlock::GunShotLineTrace(FVector TraceStart, FVector TraceEnd)
{
	FHitResult Hit;
	FCollisionQueryParams queryParam;
	queryParam.AddIgnoredActor(this);
	queryParam.AddIgnoredComponent(FP_Gun);
	DrawDebugLine(GetWorld(), TraceStart, TraceEnd, FColor::Red, false, 5.0f);
}

void AWeaponGlock::OnReload(USkeletalMeshComponent* SkMesh)
{
	// 重新计算子弹数量
	// 如果子弹小于一个弹夹就不换弹
	if (MaxAmmoCount < 20)
	{
		AmmoCount = MaxAmmoCount;
		MaxAmmoCount = 0;
		MagazineAmmo = 0;
		return;
	}

	// 播放动画和声音
	MaxAmmoCount = MaxAmmoCount - 20 + AmmoCount;

	//// Load static asset
	FString GlockReloadAnimation = FString(TEXT("AnimSequence'/Game/ShootingPawn/Animations/Glock_Reload_anim.Glock_Reload_anim'"));
	UAnimationAsset* assetAnim = Cast<UAnimationAsset>(LoadObject<UAnimationAsset>(nullptr, *GlockReloadAnimation));

	if (assetAnim != nullptr)
	{
		FP_Gun->PlayAnimation(assetAnim, false);
	}

	AmmoCount = MagazineAmmo;
	AShootingHUD* hud = Cast<AShootingHUD>(UGameplayStatics::GetPlayerController(this, 0)->GetHUD());
	hud->UpdateAmmo(AmmoCount, MagazineAmmo, MaxAmmoCount);
}

bool AWeaponGlock::OnCheckAmmo()
{
	if (AmmoCount <= 0)
	{
		// 播放声音
		FString DryFireSoundPath = FString(TEXT("SoundWave'/Game/ShootingPawn/Sounds/DryFire.DryFire'"));
		DryFireSound = Cast<USoundBase>(LoadObject<USoundBase>(nullptr, *DryFireSoundPath));
		if (DryFireSound != nullptr)
		{
			UGameplayStatics::PlaySoundAtLocation(this, DryFireSound, GetActorLocation());
		}
		return false;
	}

	return true;
}


//void AWeaponGlock::SpawnBulletDecalTrace(FVector Location)
//{
//	//Blueprint
//	UClass* BulletDecalClass = LoadClass<AActor>(nullptr, TEXT("Blueprint'/Game/ShootingPawn/Blueprints/BulletDecal_BP.BulletDecal_BP_C'"));
//	UClass* BulletImpactClass = LoadClass<AActor>(nullptr, TEXT("Blueprint'/Game/ShootingPawn/Blueprints/BulletImpacts_BP.BulletImpacts_BP_C'"));
//
//	UWorld* const World = GetWorld();
//
//	if (BulletDecalClass != nullptr && BulletImpactClass != nullptr)
//	{
//		if (World != nullptr)
//		{
//			//ImpactPoint
//			FRotator Rotator1 = UKismetMathLibrary::MakeRotFromX(Location);
//			AActor* BulletDecal = World->SpawnActor<AActor>(BulletDecalClass, Location, Rotator1);
//
//			AActor* BulletImpact = World->SpawnActor<AActor>(BulletImpactClass, Location, Rotator1);
//		}
//	}
//}


//void AWeaponGlock::SpawnTraceRounder(FVector Location, FVector SpawnTransFormLocation, FVector ImpactPoint)
//{
//	//Blueprint
//	UClass* TraceRoundClass = LoadClass<AShootingProjectile>(nullptr, TEXT("Blueprint'/Game/ShootingPawn/Blueprints/ShootingBullet_BP.ShootingBullet_BP_C'"));
//	UWorld* const World = GetWorld();
//
//	AShootingCharacter* MyPawn = Cast<AShootingCharacter>(UGameplayStatics::GetPlayerCharacter(GetWorld(), 0));
//	UCameraComponent* FirstCamera = MyPawn->FirstPersonCameraComponent;
//	FRotator CameraRotator = FirstCamera->GetComponentRotation();
//	auto Matix = UKismetMathLibrary::MakeTransform(Location, CameraRotator, FVector(1, 1, 1));
//
//	AShootingProjectile* Tracer = World->SpawnActor<AShootingProjectile>(TraceRoundClass, Matix);
//	if (!Tracer)
//	{
//		GEngine->AddOnScreenDebugMessage(-1, 10.0f, FColor::Green, TEXT("Tracer NULL"));
//	}
//}

void AWeaponGlock::MuzzleFlash()
{
	FP_PointLight->SetIntensity(20000.0f);
	FP_FlashPlane->SetVisibility(true);

	// Delay 0.1
	FLatentActionInfo LatentInfo;
	LatentInfo.Linkage = 0;
	LatentInfo.CallbackTarget = this;
	LatentInfo.ExecutionFunction = "DelayAndDisplayMuzzle";
	LatentInfo.UUID = __LINE__;//行号为ID
	UKismetSystemLibrary::Delay(this, 0.1f, LatentInfo);
}

void AWeaponGlock::DelayAndDisplayMuzzle()
{
	FP_PointLight->SetIntensity(0);
	FP_FlashPlane->SetVisibility(false);

	float RotatorValue = FMath::RandRange(60, 120);
	FP_FlashPlane->SetRelativeRotation(FRotator(90, RotatorValue, 90));
	float ScaleValue = FMath::RandRange(0.1f, 0.3f);
	FP_FlashPlane->SetWorldScale3D(FVector(ScaleValue, ScaleValue, ScaleValue));
}