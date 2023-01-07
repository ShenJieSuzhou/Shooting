// Fill out your copyright notice in the Description page of Project Settings.


#include "WeaponKnife.h"

AWeaponKnife::AWeaponKnife()
{
	auto assetSkeletal = ConstructorHelpers::FObjectFinder<USkeletalMesh>(TEXT("StaticMesh'/Game/ShootingPawn/Meshs/AK_mesh.AK_mesh'"));
	//USkeletalMesh SkeletalMesh = LoadObject<USkeletalMesh>(NULL, TEXT("SkeletalMesh'/Game/TwinSwordAnimsetBase/UE4_Mannequin/Mesh/SK_Mannequin.SK_Mannequin'"));
	FP_Gun->SetSkeletalMesh(assetSkeletal.Object);
}


