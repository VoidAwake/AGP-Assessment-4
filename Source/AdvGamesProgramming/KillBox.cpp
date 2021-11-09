// Fill out your copyright notice in the Description page of Project Settings.


#include "KillBox.h"

#include "PlayerCharacter.h"
#include "Components/StaticMeshComponent.h"

// Sets default values
AKillBox::AKillBox()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	KillBoxComponent = CreateDefaultSubobject<UBoxComponent>(TEXT("AAA Component"));
	KillBoxComponent->SetCollisionProfileName("Trigger");
	RootComponent = KillBoxComponent;

	KillBoxMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("SS Mesh"));
	KillBoxMeshComponent->SetupAttachment(RootComponent);
	
	KillBoxComponent->SetGenerateOverlapEvents(true);

}

// Called when the game starts or when spawned
void AKillBox::BeginPlay()
{
	Super::BeginPlay();
	KillBoxComponent->OnComponentBeginOverlap.AddDynamic(this,&AKillBox::OnOverlapBegin);

	KillBoxComponent->OnComponentEndOverlap.AddDynamic(this,&AKillBox::OnOverlapEnd);
}

// Called every frame
void AKillBox::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AKillBox::OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComponent, int32 OtherBodyIndex, bool bFromSweep,
	const FHitResult& SweepResult)
{
	UE_LOG(LogTemp,Error,TEXT("OVERLAP HAS BEGUN NJADEOJKIASDJLNADSNLUJADSOHUJ"))
	APlayerCharacter* character = Cast<APlayerCharacter>(OtherActor);

	
	if (character && !flag)
	{
		flag = true;
		character->OnDeath();
	}

	flag = false;
}

void AKillBox::OnOverlapEnd(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
}

