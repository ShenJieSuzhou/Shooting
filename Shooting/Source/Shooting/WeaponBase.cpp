// Fill out your copyright notice in the Description page of Project Settings.


#include "WeaponBase.h"
#include "Kismet/KismetMathLibrary.h"
#include "CusActor/BulletHole.h"
#include "CusActor/BulletImpactEffect.h"
#include "Util/ShootingUtil.h"
// Sets default values
AWeaponBase::AWeaponBase()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// Set Gun
	FP_Gun = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("FP_Gun"));
	FP_Gun->SetOnlyOwnerSee(false);
	FP_Gun->bCastDynamicShadow = false;
	FP_Gun->CastShadow = false;
	FP_Gun->SetupAttachment(RootComponent);

	// Set MuzzleLocation
	FP_MuzzleLocation = CreateDefaultSubobject<USceneComponent>(TEXT("MuzzleLocation"));
	FP_MuzzleLocation->SetupAttachment(FP_Gun);

	GunOffset = FVector(100.0f, 0.0f, 10.0f);

	// Set PointLight
	FP_PointLight = CreateDefaultSubobject<UPointLightComponent>(TEXT("PointLight"));
	FP_PointLight->SetIntensity(0);
	FP_PointLight->SetupAttachment(FP_Gun);

	// SetPlane
	FP_FlashPlane = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("GunFlash"));
	FP_FlashPlane->SetVisibility(false);
	FP_FlashPlane->SetupAttachment(FP_Gun);
}

// Called when the game starts or when spawned
void AWeaponBase::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AWeaponBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}


void AWeaponBase::SpawnBulletDecalTrace(FHitResult Hit)
{
	// 判断击中的是什么物体，然后生成不同的效果
	EPhysicalSurface SurfaceType = FShootingUtil::GetInstance()->GetPhysicalSurfaceType(Hit.PhysMaterial.Get());  
	FString BulletHolePath = FShootingUtil::GetInstance()->RandomGenerateBulletHole(SurfaceType);
	FString ImpactParticlePath = FShootingUtil::GetInstance()->GetImpactParticleSyatem(SurfaceType);

	//Blueprint
	UClass* BulletDecalClass = LoadClass<ABulletHole>(nullptr, TEXT("Class'/Script/Shooting.BulletHole'"));
	UClass* BulletImpactClass = LoadClass<ABulletImpactEffect>(nullptr, TEXT("Class'/Script/Shooting.BulletImpactEffect'"));

	UWorld* const World = GetWorld();

	if (BulletDecalClass != nullptr && BulletImpactClass != nullptr)
	{
		if (World != nullptr)
		{
			FRotator Rotator1 = UKismetMathLibrary::MakeRotFromX(Hit.ImpactNormal);
			if(!BulletHolePath.IsEmpty())
			{
				ABulletHole* BulletDecal = World->SpawnActor<ABulletHole>(BulletDecalClass, Hit.Location, Rotator1);
				BulletDecal->SetBulletHoleMaterial(BulletHolePath);
			}
			
			if(!ImpactParticlePath.IsEmpty())
			{
				ABulletImpactEffect* BulletImpact = World->SpawnActor<ABulletImpactEffect>(BulletImpactClass, Hit.Location, Rotator1);
				BulletImpact->SetNiagaraSysAsset(ImpactParticlePath);
			}
		}
	}
}
