// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/TextBlock.h"
#include "AmmoWidget.generated.h"

/**
 * 
 */
UCLASS()
class SHOOTING_API UAmmoWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	class UTextBlock* CurrentAmmo;

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	class UTextBlock* MagazineAmmo;

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	class UTextBlock* TotalAmmo;

};
