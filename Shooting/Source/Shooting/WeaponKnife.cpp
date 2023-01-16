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

void AWeaponKnife::OnFire(USkeletalMeshComponent* SkMesh)
{
	// Load static asset
	FString knifeAttackMontage = FString(TEXT("AnimMontage'/Game/ShootingPawn/Animations/Arms_Knife_Attack_01_anim_Montage.Arms_Knife_Attack_01_anim_Montage'"));
	UAnimMontage* assetMontage = Cast<UAnimMontage>(LoadObject<UAnimMontage>(nullptr, *knifeAttackMontage));
	FireAnimation = assetMontage;

	//FireAnimation = assetMontage.Object;
	if (FireAnimation != nullptr)
	{
		// Get the animation object for the arms mesh
		UAnimInstance* AnimInstance = SkMesh->GetAnimInstance();
		if (AnimInstance != nullptr)
		{
			AnimInstance->Montage_Play(FireAnimation, 1.f);
		}
	}

	// try and play the sound if specified
//if (FireSound != nullptr)
//{
//	UGameplayStatics::PlaySoundAtLocation(this, FireSound, GetActorLocation());
//}

// try and play a firing animation if specified
// auto assetMontage = ConstructorHelpers::FObjectFinder<UAnimMontage>(TEXT("AnimMontage'/Game/ShootingPawn/Animations/Arms_Knife_Attack_01_anim_Montage.Arms_Knife_Attack_01_anim_Montage'"));
//const ConstructorHelpers::FObjectFinder<UAnimMontage> Combo1Finder = ConstructorHelpers::FObjectFinder<UAnimMontage>(TEXT("AnimMontage'/Game/ShootingPawn/Animations/Arms_Knife_Attack_01_anim_Montage.Arms_Knife_Attack_01_anim_Montage'"));
//if (Combo1Finder.Succeeded())
//{
//	FireAnimation = Combo1Finder.Object;
//}
}

void AWeaponKnife::OnReload(USkeletalMeshComponent* SkMesh)
{

}


