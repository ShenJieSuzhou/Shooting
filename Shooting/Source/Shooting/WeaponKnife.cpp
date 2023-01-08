// Fill out your copyright notice in the Description page of Project Settings.


#include "WeaponKnife.h"

AWeaponKnife::AWeaponKnife()
{
    // get knife mesh
	if(FP_Gun)
	{
		auto assetSkeletal = ConstructorHelpers::FObjectFinder<USkeletalMesh>(TEXT("SkeletalMesh'/Game/ShootingPawn/rigged/Knife_Rigged.Knife_Rigged'"));
		if(assetSkeletal.Succeeded())
		{
			FP_Gun->SetSkeletalMesh(assetSkeletal.Object);
		}
		
	}

}


