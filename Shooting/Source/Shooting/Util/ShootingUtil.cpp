// Fill out your copyright notice in the Description page of Project Settings.


#include "ShootingUtil.h"

// 获取击中的材质
EPhysicalSurface FShootingUtil::GetPhysicalSurfaceType(UPhysicalMaterial* material)
{
	if(material == NULL)
	{
		return EPhysicalSurface::SurfaceType_Default;
	}

	return UPhysicalMaterial::DetermineSurfaceType(material);
}

FString FShootingUtil::RandomGenerateBulletHole(EPhysicalSurface type)
{
	TArray<FString> BulletHoles = FShootingConfigs::GetInstance()->GetBulletDecalWithType(type);
	int32 ArrayLength = BulletHoles.Num();
	if(ArrayLength == 0)
	{
		return TEXT("");
	}

	int32 index = FMath::RandRange(0, ArrayLength - 1);
	return BulletHoles[index];
}

// 获取击中粒子效果
FString FShootingUtil::GetImpactParticleSyatem(EPhysicalSurface type)
{
	FString ParticlePath = FShootingConfigs::GetInstance()->GetParticleSystemWithType(type);
	if(ParticlePath.IsEmpty())
	{
		return TEXT("");
	}

	return ParticlePath;
}




