// Fill out your copyright notice in the Description page of Project Settings.


#include "PickUpWeaponBase.h"

// Sets default values
APickUpWeaponBase::APickUpWeaponBase()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// Set Static Mesh
	StaticMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("StaticMesh"));
	StaticMesh->SetupAttachment(RootComponent);

	// Set SphereCollision
	Sphere = CreateDefaultSubobject<USphereComponent>(TEXT("Sphere"));
	Sphere->SetupAttachment(StaticMesh);
}

// Called when the game starts or when spawned
void APickUpWeaponBase::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void APickUpWeaponBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

