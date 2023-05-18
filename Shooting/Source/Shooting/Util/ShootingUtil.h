// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "PhysicalMaterials/PhysicalMaterial.h"
#include "Chaos/ChaosEngineInterface.h"
#include "../GameConfig/ShootingConfigs.h"
#include "Math/UnrealMathUtility.h"

class FShootingUtil
{
public:
	static FShootingUtil* GetInstance()
	{
		static FShootingUtil sFShootingUtil;
		return &sFShootingUtil;
	}

	// ��ȡ���еĲ���
	EPhysicalSurface GetPhysicalSurfaceType(UPhysicalMaterial* material);

	// ���ݲ���������ɵ���
	FString RandomGenerateBulletHole(EPhysicalSurface type);

	// ��ȡ��������Ч��
	FString GetImpactParticleSyatem(EPhysicalSurface type);
};