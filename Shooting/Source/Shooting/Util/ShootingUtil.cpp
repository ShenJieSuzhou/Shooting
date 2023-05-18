// Fill out your copyright notice in the Description page of Project Settings.


#include "ShootingUtil.h"

// ��ȡ���еĲ���
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

// ��ȡ��������Ч��
FString FShootingUtil::GetImpactParticleSyatem(EPhysicalSurface type)
{
	FString ParticlePath = FShootingConfigs::GetInstance()->GetParticleSystemWithType(type);
	if(ParticlePath.IsEmpty())
	{
		return TEXT("");
	}

	return ParticlePath;
}




