// Fill out your copyright notice in the Description page of Project Settings.


#include "AmmoWidget.h"

void UAmmoWidget::UpdateAmmo(int current, int Magazine, int maxAmmount)
{	
	FString sCurrent = FString::FromInt(current);
	CurrentAmmo->SetText(FText::FromString(sCurrent));

	FString sMagazine = FString::FromInt(Magazine);
	MagazineAmmo->SetText(FText::FromString(sMagazine));

	FString sMaxAmmount = FString::FromInt(maxAmmount);
	TotalAmmo->SetText(FText::FromString(sMaxAmmount));
}