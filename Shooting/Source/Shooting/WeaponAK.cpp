// Fill out your copyright notice in the Description page of Project Settings.


#include "WeaponAK.h"
#include "ShootingHUD.h"
#include "ShootingCharacter.h"
#include "DrawDebugHelpers.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
#include "Math/UnrealMathUtility.h"

AWeaponAK::AWeaponAK()
{
    // get knife mesh
	if(FP_Gun)
	{
		auto assetSkeletal = ConstructorHelpers::FObjectFinder<USkeletalMesh>(TEXT("SkeletalMesh'/Game/ShootingPawn/rigged/AK_rigged.AK_rigged'"));
		if(assetSkeletal.Succeeded())
		{
			FP_Gun->SetSkeletalMesh(assetSkeletal.Object);
			FP_MuzzleLocation->SetWorldLocation(FVector(68.2f, -2.0f, 9.0f));
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
	// 播放声音


	// Load static asset
	FString AkFireAnimation = FString(TEXT("AnimSequence'/Game/ShootingPawn/Animations/Arms_AK_Fire_anim.Arms_AK_Fire_anim'"));
	UAnimationAsset* assetAnim = Cast<UAnimationAsset>(LoadObject<UAnimationAsset>(nullptr, *AkFireAnimation));

	if (assetAnim != nullptr)
	{
		SkMesh->PlayAnimation(assetAnim, false);
	}

	//发射子弹

	AmmoCount--;
	AShootingHUD* hud = Cast<AShootingHUD>(UGameplayStatics::GetPlayerController(this, 0)->GetHUD());
	hud->UpdateAmmo(AmmoCount, MagazineAmmo, MaxAmmoCount);

	// Camera Shot
	CameraShotLineTrace();
}

void AWeaponAK::CameraShotLineTrace()
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
	DrawDebugLine(GetWorld(), TraceStart, TraceEnd, isHit ? FColor::Red : FColor::Green, false, 5.0f);

	if (isHit)
	{
		UE_LOG(LogTemp, Log, TEXT("Trace hit actor"));
		FVector start = FP_MuzzleLocation->GetComponentLocation();
		GunShotLineTrace(start, Hit.ImpactPoint);
		SpawnBulletDecalTrace(FP_MuzzleLocation->GetComponentLocation(), Hit.ImpactPoint, Hit.ImpactPoint);
	}
	else
	{
		UE_LOG(LogTemp, Log, TEXT("No Actors were hit"));
	}


}

void AWeaponAK::GunShotLineTrace(FVector TraceStart, FVector TraceEnd)
{
	FHitResult Hit;
	FCollisionQueryParams queryParam;
	queryParam.AddIgnoredActor(this);
	queryParam.AddIgnoredComponent(FP_Gun);
	DrawDebugLine(GetWorld(), TraceStart, TraceEnd, FColor::Red, false, 5.0f);
}

void AWeaponAK::OnReload(USkeletalMeshComponent* SkMesh)
{
	// 重新计算子弹数量
	// 如果子弹小于一个弹夹就不换弹
	if(MaxAmmoCount < 30)
	{
		// 播放动画和声音

		AmmoCount = MaxAmmoCount;
		MaxAmmoCount = 0;
		MagazineAmmo = 0;


		return;
	}

	// 播放动画和声音
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

void AWeaponAK::SpawnBulletDecalTrace(FVector Location, FVector SpawnTransFormLocation, FVector ImpactPoint)
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
			World->SpawnActor<AActor>(BulletDecalClass, SpawnTransFormLocation, Rotator1);
		}
	}
}