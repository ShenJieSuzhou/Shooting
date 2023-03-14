// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/StaticMeshComponent.h"
#include "Components/SphereComponent.h"
#include "WeaponBase.h"
#include "PickUpWeaponBase.generated.h"

UCLASS()
class SHOOTING_API APickUpWeaponBase : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	APickUpWeaponBase();

	UPROPERTY(VisibleDefaultsOnly, Category = Mesh)
	UStaticMeshComponent* StaticMesh;

	UPROPERTY(VisibleDefaultsOnly, Category = Mesh)
	USphereComponent* Sphere;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Gameplay)
	EWeapon WeaponType;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
