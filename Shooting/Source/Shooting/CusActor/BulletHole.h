// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/DecalComponent.h"
#include "BulletHole.generated.h"

UCLASS()
class SHOOTING_API ABulletHole : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ABulletHole();

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Decal")
	UDecalComponent* Decal;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	void SetBulletHoleMaterial(UMaterialInterface* material);

};
