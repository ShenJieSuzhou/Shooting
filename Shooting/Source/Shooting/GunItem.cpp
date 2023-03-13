// Fill out your copyright notice in the Description page of Project Settings.


#include "GunItem.h"

UGunItem::~UGunItem()
{
	Owned = nullptr;
	NotOwned = nullptr;
}

void UGunItem::SetImage(UTexture2D* selected, UTexture2D* UnSelected)
{
	Owned = selected;
	NotOwned = UnSelected;
}

void UGunItem::SetIsOwned(bool isOwned)
{
	if(isOwned)
	{
		WeaponImage->SetBrushFromTexture(Owned);
	}
	else
	{
		WeaponImage->SetBrushFromTexture(NotOwned);
	}
}