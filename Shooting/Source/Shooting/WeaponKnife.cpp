// Fill out your copyright notice in the Description page of Project Settings.


#include "WeaponKnife.h"

AWeaponKnife::AWeaponKnife()
{
    // get knife mesh
 	auto assetSkeletal = ConstructorHelpers::FObjectFinder<USkeletalMesh>(TEXT("StaticMesh'/Game/ShootingPawn/Meshs/Knife_mesh.Knife_mesh'"));
	FP_Gun->SetSkeletalMesh(assetSkeletal.Object);
}


