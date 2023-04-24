// Fill out your copyright notice in the Description page of Project Settings.


#include "WeaponAWP.h"

#include "DrawDebugHelpers.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
#include "Math/UnrealMathUtility.h"
#include "ShootingHUD.h"
#include "ShootingCharacter.h"

AWeaponAWP::AWeaponAWP()
{
    // get knife mesh
	if(FP_Gun)
	{
		auto assetSkeletal = ConstructorHelpers::FObjectFinder<USkeletalMesh>(TEXT("SkeletalMesh'/Game/ShootingPawn/rigged/AWP_Rigged.AWP_Rigged'"));
		if(assetSkeletal.Succeeded())
		{
			FP_Gun->SetSkeletalMesh(assetSkeletal.Object);
			FP_MuzzleLocation->SetWorldLocation(FVector(109.0f, -2.0f, 9.0f));
			FP_PointLight->SetWorldLocation(FVector(39.0f, -5.0f, 5.f));

			FString GunMuzzle = FString(TEXT("StaticMesh'/Game/ShootingPawn/Meshs/FlashPlane.plane_ground_3x3'"));
			UStaticMesh* mesh = Cast<UStaticMesh>(LoadObject<UStaticMesh>(nullptr, *GunMuzzle));
			FP_FlashPlane->SetStaticMesh(mesh);
			FP_FlashPlane->SetCollisionEnabled(ECollisionEnabled::NoCollision);
			FP_FlashPlane->SetWorldRotation(FRotator(90.f, 90.f, 90.f));
			FP_FlashPlane->SetWorldLocation(FVector(109.f, 1, 8.37f));
			FP_FlashPlane->SetWorldScale3D(FVector(0.15f, 0.15f, 0.15f));
		}
	}

	// 最大弹夹量
	MaxAmmoCount = 40;
	// 单个弹夹弹量
	AmmoCount = 8;
	// 弹夹装载量
	MagazineAmmo = 8;
	// 子弹精度
	BulletSpread = 0.8;
	// 装弹时间
	ReloadTime = 1.5;
	// 是否上膛
	IsReady = true;
}

void AWeaponAWP::OnFire(USkeletalMeshComponent* SkMesh, bool isAimDown)
{
	if (!IsReady)
	{
		return;
	}

	IsReady = false;
	// Delay 1.33
	FLatentActionInfo LatentInfo;
	LatentInfo.Linkage = 0;
	LatentInfo.CallbackTarget = this;
	LatentInfo.ExecutionFunction = "BulletReloaded";
	LatentInfo.UUID = __LINE__;//行号为ID
	UKismetSystemLibrary::Delay(this, 1.33f, LatentInfo);

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
		FString GlockFireMontage = FString(TEXT("AnimMontage'/Game/ShootingPawn/Animations/Arms_AWP_Fire_anim_Montage.Arms_AWP_Fire_anim_Montage'"));
		UAnimMontage* assetMontage = Cast<UAnimMontage>(LoadObject<UAnimMontage>(nullptr, *GlockFireMontage));
		FireAnimation = assetMontage;

		if (FireAnimation != nullptr)
		{
			// Get the animation object for the arms mesh
			UAnimInstance* AnimInstance = SkMesh->GetAnimInstance();
			if (AnimInstance != nullptr)
			{
				AnimInstance->Montage_Play(FireAnimation, 1.0f);
			}
		}
	}
	else
	{
		// Load static asset
		//FString GlockFireMontage = FString(TEXT("AnimMontage'/Game/ShootingPawn/Animations/Arms_Glock_ADS_Fire_anim_Montage.Arms_Glock_ADS_Fire_anim_Montage'"));
		//UAnimMontage* assetMontage = Cast<UAnimMontage>(LoadObject<UAnimMontage>(nullptr, *GlockFireMontage));
		//FireAnimation = assetMontage;

		//if (FireAnimation != nullptr)
		//{
		//	// Get the animation object for the arms mesh
		//	UAnimInstance* AnimInstance = SkMesh->GetAnimInstance();
		//	if (AnimInstance != nullptr)
		//	{
		//		AnimInstance->Montage_Play(FireAnimation, 0.8f);
		//	}
		//}
	}
	
	// Load static asset
	//FString AWPFireAnimation = FString(TEXT("AnimSequence'/Game/ShootingPawn/Animations/AWP_Fire_anim.AWP_Fire_anim'"));
	//UAnimationAsset* assetAnim = Cast<UAnimationAsset>(LoadObject<UAnimationAsset>(nullptr, *AWPFireAnimation));
	//if (assetAnim != nullptr)
	//{
	//	FP_Gun->PlayAnimation(assetAnim, false);
	//}

	//发射子弹
	AmmoCount--;
	AShootingHUD* hud = Cast<AShootingHUD>(UGameplayStatics::GetPlayerController(this, 0)->GetHUD());
	hud->UpdateAmmo(AmmoCount, MagazineAmmo, MaxAmmoCount);

	// Camera Shot
	CameraShotLineTrace();
}

void AWeaponAWP::OnStopFire()
{

}


void AWeaponAWP::CameraShotLineTrace()
{
	AShootingCharacter* MyPawn = Cast<AShootingCharacter>(UGameplayStatics::GetPlayerCharacter(GetWorld(), 0));
	UCameraComponent* FirstCamera = MyPawn->FirstPersonCameraComponent;
	FVector TraceStart = FirstCamera->GetComponentLocation();
	FVector TraceEnd = FirstCamera->GetForwardVector() * 200000;

	float calu = BulletSpread * -1;
	float x = FMath::RandRange(calu, BulletSpread);
	float y = FMath::RandRange(calu, BulletSpread);
	float z = FMath::RandRange(calu, BulletSpread);
	TraceEnd = TraceEnd + TraceStart + FVector(x, y, z);

	FHitResult Hit;
	FCollisionQueryParams queryParam;
	queryParam.AddIgnoredActor(this);
	bool isHit = GetWorld()->LineTraceSingleByChannel(Hit, TraceStart, TraceEnd, ECC_Visibility, queryParam);
	//DrawDebugLine(GetWorld(), TraceStart, TraceEnd, isHit ? FColor::Red : FColor::Green, false, 5.0f);

	if (isHit)
	{
		UE_LOG(LogTemp, Log, TEXT("Trace hit actor"));
		FVector start = FP_MuzzleLocation->GetComponentLocation();
		//GunShotLineTrace(start, Hit.ImpactPoint);
		SpawnBulletDecalTrace(FP_MuzzleLocation->GetComponentLocation(), Hit.ImpactPoint, Hit.ImpactPoint);

		int32 value = FMath::RandRange(1, 10);
		if (value >= 0)
		{
			SpawnTraceRounder(FP_MuzzleLocation->GetComponentLocation(), Hit.ImpactPoint, Hit.ImpactPoint);
			MuzzleFlash();
		}
	}
	else
	{
		UE_LOG(LogTemp, Log, TEXT("No Actors were hit"));
	}
}

void AWeaponAWP::GunShotLineTrace(FVector TraceStart, FVector TraceEnd)
{
	FHitResult Hit;
	FCollisionQueryParams queryParam;
	queryParam.AddIgnoredActor(this);
	queryParam.AddIgnoredComponent(FP_Gun);
	DrawDebugLine(GetWorld(), TraceStart, TraceEnd, FColor::Red, false, 5.0f);
}

void AWeaponAWP::OnReload(USkeletalMeshComponent* SkMesh)
{
	// 重新计算子弹数量
	// 如果子弹小于一个弹夹就不换弹
	if (MaxAmmoCount < 8)
	{
		AmmoCount = MaxAmmoCount;
		MaxAmmoCount = 0;
		MagazineAmmo = 0;
		return;
	}

	// 播放动画和声音
	MaxAmmoCount = MaxAmmoCount - 8 + AmmoCount;

	//// Load static asset
	FString AWPReloadAnimation = FString(TEXT("AnimSequence'/Game/ShootingPawn/Animations/AWP_Reload_anim.AWP_Reload_anim'"));
	UAnimationAsset* assetAnim = Cast<UAnimationAsset>(LoadObject<UAnimationAsset>(nullptr, *AWPReloadAnimation));

	if (assetAnim != nullptr)
	{
		FP_Gun->PlayAnimation(assetAnim, false);
	}

	AmmoCount = MagazineAmmo;
	AShootingHUD* hud = Cast<AShootingHUD>(UGameplayStatics::GetPlayerController(this, 0)->GetHUD());
	hud->UpdateAmmo(AmmoCount, MagazineAmmo, MaxAmmoCount);
}

bool AWeaponAWP::OnCheckAmmo()
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


void AWeaponAWP::SpawnBulletDecalTrace(FVector Location, FVector SpawnTransFormLocation, FVector ImpactPoint)
{
	//Blueprint
	UClass* BulletDecalClass = LoadClass<AActor>(nullptr, TEXT("Blueprint'/Game/ShootingPawn/Blueprints/BulletDecal_BP.BulletDecal_BP_C'"));

	UWorld* const World = GetWorld();
	FRotator Rotator = FRotator(0.f);

	if (BulletDecalClass != nullptr)
	{
		if (World != nullptr)
		{
			//ImpactPoint
			FRotator Rotator1 = UKismetMathLibrary::MakeRotFromX(ImpactPoint);
			AActor* BulletDecal = World->SpawnActor<AActor>(BulletDecalClass, SpawnTransFormLocation, Rotator1);
			BulletDecal->SetActorScale3D(FVector(0.025f));
		}
	}
}


void AWeaponAWP::SpawnTraceRounder(FVector Location, FVector SpawnTransFormLocation, FVector ImpactPoint)
{
	//Blueprint
	UClass* TraceRoundClass = LoadClass<AActor>(nullptr, TEXT("Blueprint'/Game/ShootingPawn/Blueprints/TraceRound_BP.TraceRound_BP_C'"));
	UWorld* const World = GetWorld();

	AShootingCharacter* MyPawn = Cast<AShootingCharacter>(UGameplayStatics::GetPlayerCharacter(GetWorld(), 0));
	UCameraComponent* FirstCamera = MyPawn->FirstPersonCameraComponent;
	FRotator CameraRotator = FirstCamera->GetComponentRotation();
	auto Matix = UKismetMathLibrary::MakeTransform(Location, CameraRotator, FVector(1, 1, 1));

	AActor* Tracer = World->SpawnActor<AActor>(TraceRoundClass, Matix);
	if (!Tracer)
	{
		GEngine->AddOnScreenDebugMessage(-1, 10.0f, FColor::Green, TEXT("Tracer NULL"));
	}
}

void AWeaponAWP::MuzzleFlash()
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

void AWeaponAWP::DelayAndDisplayMuzzle()
{
	FP_PointLight->SetIntensity(0);
	FP_FlashPlane->SetVisibility(false);

	float RotatorValue = FMath::RandRange(60, 120);
	FP_FlashPlane->SetRelativeRotation(FRotator(90, RotatorValue, 90));
	float ScaleValue = FMath::RandRange(0.1f, 0.3f);
	FP_FlashPlane->SetWorldScale3D(FVector(ScaleValue, ScaleValue, ScaleValue));
}

void AWeaponAWP::BulletReloaded()
{
	IsReady = true;
}