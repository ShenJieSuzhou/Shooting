// Fill out your copyright notice in the Description page of Project Settings.


#include "AmmoWidget.h"

bool UAmmoWidget::Initialize()
{
	Super::Initialize();

	return true;
}

void UAmmoWidget::InitItems()
{
	if (RifleLight)
	{
		FString selPath = FString(TEXT("Texture2D'/Game/ShootingPawn/Textures/ak-47.ak-47'"));
		FString UnSelPath = FString(TEXT("Texture2D'/Game/ShootingPawn/Textures/ak-47-gray.ak-47-gray'"));
		UTexture2D* selTexture = LoadObject<UTexture2D>(nullptr, *selPath);
		UTexture2D* unSelTexture = LoadObject<UTexture2D>(nullptr, *UnSelPath);
		if (selTexture != nullptr && unSelTexture != nullptr)
		{
			RifleLight->SetImage(selTexture, unSelTexture);
		}
	}

	if (PistoLight)
	{
		FString selPath = FString(TEXT("Texture2D'/Game/ShootingPawn/Textures/pisto.pisto'"));
		FString UnSelPath = FString(TEXT("Texture2D'/Game/ShootingPawn/Textures/shooter-game-gray.shooter-game-gray'"));
		UTexture2D* selTexture = LoadObject<UTexture2D>(nullptr, *selPath);
		UTexture2D* unSelTexture = LoadObject<UTexture2D>(nullptr, *UnSelPath);
		if (selTexture != nullptr && unSelTexture != nullptr)
		{
			PistoLight->SetImage(selTexture, unSelTexture);
		}
	}

	if (KnifeLight)
	{
		FString selPath = FString(TEXT("Texture2D'/Game/ShootingPawn/Textures/knife.knife'"));
		FString UnSelPath = FString(TEXT("Texture2D'/Game/ShootingPawn/Textures/army-knife-gray.army-knife-gray'"));
		UTexture2D* selTexture = LoadObject<UTexture2D>(nullptr, *selPath);
		UTexture2D* unSelTexture = LoadObject<UTexture2D>(nullptr, *UnSelPath);
		if (selTexture != nullptr && unSelTexture != nullptr)
		{
			KnifeLight->SetImage(selTexture, unSelTexture);
		}

	}

	RifleLight->SetIsOwned(false);
	PistoLight->SetIsOwned(false);
	KnifeLight->SetIsOwned(false);
}

void UAmmoWidget::UpdateAmmo(int current, int Magazine, int maxAmmount)
{	
	FString sCurrent = FString::FromInt(current);
	CurrentAmmo->SetText(FText::FromString(sCurrent));

	FString sMagazine = FString::FromInt(Magazine);
	MagazineAmmo->SetText(FText::FromString(sMagazine));

	FString sMaxAmmount = FString::FromInt(maxAmmount);
	TotalAmmo->SetText(FText::FromString(sMaxAmmount));
}

void UAmmoWidget::UpdateWeaponsInventory(GunInventory* inventory)
{
	RifleLight->SetIsEnabled(false);
}
