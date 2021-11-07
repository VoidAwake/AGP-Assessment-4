// Fill out your copyright notice in the Description page of Project Settings.


#include "GravityObject.h"

#include "Components/StaticMeshComponent.h"

// Sets default values
AGravityObject::AGravityObject()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	radius = 100.0f;
	SphereComponent = CreateDefaultSubobject<USphereComponent>(TEXT("Sphere Component"));
	SphereComponent->InitSphereRadius(radius);
	SphereComponent->SetCollisionProfileName("PhysicsActor");
	SphereComponent->SetGenerateOverlapEvents(true);
	RootComponent = SphereComponent;
	

	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	Mesh->SetupAttachment(RootComponent);

	
	
}

// Called when the game starts or when spawned
void AGravityObject::BeginPlay()
{
	Super::BeginPlay();
	FlagStartGravityPush = false;
	
}

// Called every frame
void AGravityObject::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}
