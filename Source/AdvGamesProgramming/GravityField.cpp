// Fill out your copyright notice in the Description page of Project Settings.


#include "GravityField.h"

#include "DrawDebugHelpers.h"
#include "GravComponent.h"
#include "GravityObject.h"
//#include "GravityShooterCharacter.h"
#include "Components/SphereComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Engine/Engine.h"
#include "Materials/MaterialInstance.h"
#include "Materials/MaterialInstanceDynamic.h"

FGravityFieldGenerator::FGravityFieldGenerator()
{
}

// Sets default values
AGravityField::AGravityField()
{
	
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	GravityBoxComponent = CreateDefaultSubobject<UBoxComponent>(TEXT("Gravity Component"));
	GravityBoxComponent->SetCollisionProfileName("Trigger");
	RootComponent = GravityBoxComponent;

	GravityMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("My Mesh"));
	GravityMeshComponent->SetupAttachment(RootComponent);
	
	GravityBoxComponent->SetGenerateOverlapEvents(true);

	//GravityMeshComponent->SetMaterial()

	//  GravityBoxComponent->OnComponentBeginOverlap.AddDynamic(this,&AGravityField::OnOverlapBegin);
	// //
	// GravityBoxComponent->OnComponentEndOverlap.AddDynamic(this,&AGravityField::OnOverlapEnd);

	//GravityBoxComponent->SetWorldScale3D(FVector(1.0,2.0,200.0));

}

// Called when the game starts or when spawned
void AGravityField::BeginPlay()
{
	Super::BeginPlay();
	GravityBoxComponent->OnComponentBeginOverlap.AddDynamic(this,&AGravityField::OnOverlapBegin);
	//
	GravityBoxComponent->OnComponentEndOverlap.AddDynamic(this,&AGravityField::OnOverlapEnd);


}



bool AGravityField::ShouldTickIfViewportsOnly() const
{
	return true;
}


// Called every frame
void AGravityField::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);


#pragma region GravityField Generation
	
	if (bCreateGravityField)
	{
		CreateGravityField();
		bCreateGravityField = false;
	}

	if (bGenerateGravityField)
	{
		GenerateGravityField();
	
		
		bGenerateGravityField = false;
	}

#pragma endregion
	
	for(APlayerCharacter* player : AffectedPlayers)
	{
		// GEngine->AddOnScreenDebugMessage(-1,15,FColor::Red,TEXT("flagstarted %d"),player->FlagStartGravityPush);
		// UE_LOG(LogTemp,Warning,TEXT("PUSH"));
		
		if (player->CanBeAffectedByGravityField && player->IsAffectedByGravityField && player->FlagStartGravityPush)
		{
			UE_LOG(LogTemp,Warning,TEXT("PUSH"));
			DeterminePushForPlayer(player);
		} 
	}

	for (AActor* Object : AffectedActors)
	{

		AGravityObject* GravityObject = Cast<AGravityObject>(Object);
		UGravComponent* gravComponent = Cast<UGravComponent>(Object->GetComponentByClass(UGravComponent::StaticClass()));

		if (gravComponent != nullptr)
		{
			UStaticMeshComponent* MeshComp = Cast<UStaticMeshComponent>(Object->GetComponentByClass(UStaticMeshComponent::StaticClass()));

			if (MeshComp && GravityObject->FlagStartGravityPush)
			{
				UE_LOG(LogTemp,Warning,TEXT("PUSHactor"));

				// FVector Force = (gravComponent->AntiGravity + gravComponent->LocalGravity) * MeshComp->GetMass();
				// MeshComp->AddImpulse(FVector(0,0,10),NAME_None,true);
				DeterminePushForActor(MeshComp);
			}
		}
	}

}

void AGravityField::OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComponent, int32 OtherBodyIndex, bool bFromSweep,
	const FHitResult& SweepResult) 
{
	UE_LOG(LogTemp,Warning,TEXT("OVERLAPPED"));

		if (AffectedActors.Num() >= ActorsAllowedAtOnce || AffectedPlayers.Num() >= ActorsAllowedAtOnce)
			return;
		
		
			auto character = Cast<APlayerCharacter>(OtherActor);
		
			if (character != nullptr)
			{
		
				if (character->LerpComponent != nullptr && !character->IsAffectedByGravityField 
				&& character->IsLerpable)
				{
					DetermineLerpPositionForPlayer(character);
				} 
				character->IsAffectedByGravityField = true;
				AffectedPlayers.Push(character);
				character->SetGravity(0);
			}  else
			{
		
				UGravComponent* gravComponent = Cast<UGravComponent>(OtherActor->GetComponentByClass(UGravComponent::StaticClass()));
				ULerp* LerpComponent = Cast<ULerp>(OtherActor->GetComponentByClass(ULerp::StaticClass()));
		
				
				if (LerpComponent != nullptr)
				{
					DetermineLerpPositionForActor(OtherActor,LerpComponent);
				}
		
				
				AffectedActors.Push(OtherActor);
		
				if (gravComponent != nullptr)
				{
					UStaticMeshComponent* MeshComp = Cast<UStaticMeshComponent>(OtherActor->GetComponentByClass(UStaticMeshComponent::StaticClass()));
		
					if (MeshComp != nullptr)
					{
						// FVector Force = (gravComponent->AntiGravity + gravComponent->LocalGravity) * MeshComp->GetMass();
						// MeshComp->AddForce(Force,NAME_None,false);
						DeterminePushForActor(MeshComp);
					}
				}
		
				USphereComponent* SphereComponent = Cast<USphereComponent>(OtherActor->GetComponentByClass(USphereComponent::StaticClass()));
		
				if (SphereComponent != nullptr)
				{
					SphereComponent->SetEnableGravity(false);
				}
		
			}
	
}

void AGravityField::OnOverlapEnd(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex) 
{
	
	if (AffectedActors.Num() >= ActorsAllowedAtOnce || AffectedPlayers.Num() >= ActorsAllowedAtOnce)
		return;
	
	UE_LOG(LogTemp,Warning,TEXT("OVERLAPPEDended"));
	
	auto character = Cast<APlayerCharacter>(OtherActor);
	
	if (character != nullptr)
	{
		character->PushPlayer(FVector(0,0, 1000),true);
		
		AffectedPlayers.Remove(character);
	
		character->SetGravity(1);
		character->IsAffectedByGravityField = false;
		character->FlagStartGravityPush = false;
	} else
	{
		UE_LOG(LogTemp,Warning,TEXT("STOPACTOR"));
	
		AffectedActors.Remove(OtherActor);
	
		AGravityObject* GravityObject = Cast<AGravityObject>(OtherActor);
	
		GravityObject->FlagStartGravityPush = false;
		
		USphereComponent* SphereComponent = Cast<USphereComponent>(OtherActor->GetComponentByClass(USphereComponent::StaticClass()));
	
		if (SphereComponent != nullptr)
		{
			SphereComponent->SetEnableGravity(true);
		}
	}

	
}

void AGravityField::CreateGravityField()
{
	UE_LOG(LogTemp,Warning,TEXT("STARTED CREATING GRAVITY FIELD"));

#pragma region UpdatingMaterial

	switch(GravityType)
	{
		case EGravityFieldType::VERTICAL_PUSH:
			GravityMeshComponent->SetMaterial(0,Materials[0]);
			break;
		case EGravityFieldType::VERTICAL_PULL: 
			GravityMeshComponent->SetMaterial(0,Materials[1]);
			break;
		case EGravityFieldType::HORIZONTAL_PUSH_X: 
			GravityMeshComponent->SetMaterial(0,Materials[2]);
			break;
		case EGravityFieldType::HORIZONTAL_PULL_X: 
			GravityMeshComponent->SetMaterial(0,Materials[3]);
			break;
		case EGravityFieldType::HORIZONTAL_PUSH_Y: 
			GravityMeshComponent->SetMaterial(0,Materials[2]);
			break;
		case EGravityFieldType::HORIZONTAL_PULL_Y: 
			GravityMeshComponent->SetMaterial(0,Materials[3]);
			break;
		default: ;
	}

#pragma endregion


	if (!DontUpdateSizeAndStrengthParameters)
		SetActorScale3D(FVector(Height,Width,Length));
	

}


void AGravityField::GenerateGravityField()
{

	SetActorScale3D(FVector(1,1,1));
	GravityFieldGenerator.CalculateFieldType();
	UMaterialInstanceDynamic* Instance = UMaterialInstanceDynamic::Create(Materials[0],this);
	Instance->SetVectorParameterValue("Color", GravityFieldGenerator.Color);
	GravityMeshComponent->SetMaterial(0,Instance);

	FHitResult OutHit;
	FCollisionQueryParams CollisionParams;
	CollisionParams.AddIgnoredActor(this);
	bool HitResult;
	
	switch(GravityFieldGenerator.Direction)
	{
	case EGravityDirection::Positive_X:
		
		SetPivotOffset(FVector(-39,0,0));
		
		DrawDebugLine(GetWorld(),GetActorLocation(),(GetActorForwardVector() * 
		GravityFieldGenerator.MaxDistance )+ GetActorLocation(),FColor::Blue,false,5,0,10);

		HitResult = GetWorld()->LineTraceSingleByChannel(OutHit,GetPivotOffset() + GetActorLocation(),(GetActorForwardVector() * 
		GravityFieldGenerator.MaxDistance )+ GetActorLocation(),ECC_Visibility,CollisionParams);
		
		if (HitResult && GravityFieldGenerator.StopOnActor)
		{
			// UE_LOG(LogTemp,Warning, TEXT("The Component Being Hit is: %f"), FVector::Dist
			// (GetPivotOffset() + GetActorLocation(),OutHit
			// .Location ));
			UE_LOG(LogTemp,Warning, TEXT("The Component Being Hit is: %f"), OutHit.Distance);
			SetActorRelativeScale3D(FVector((OutHit.Distance / 70) ,1,1));
			SetActorLocation(FVector(GetActorLocation().X + 39 * (OutHit.Distance / 70 ),GetActorLocation().Y , GetActorLocation().Z),false);
		} else
		{
			SetActorScale3D(FVector((GravityFieldGenerator.MaxDistance / 70)  ,1,1));
			SetActorLocation(FVector(GetActorLocation().X + 39 * (GravityFieldGenerator.MaxDistance/ 70 ),GetActorLocation().Y , GetActorLocation().Z),false);
		}		 
		break;
	case EGravityDirection::Negative_Y:
		
		SetPivotOffset(FVector(0,0,-39));
		
		DrawDebugLine(GetWorld(),GetPivotOffset() + GetActorLocation(),(-GetActorRightVector() * 
		GravityFieldGenerator.MaxDistance )+ GetActorLocation(),FColor::Blue,false,5,0,10);

		HitResult = GetWorld()->LineTraceSingleByChannel(OutHit,GetPivotOffset() + GetActorLocation(),(-GetActorRightVector() * 
		GravityFieldGenerator.MaxDistance )+ GetActorLocation(),ECC_Visibility,CollisionParams);
		
		if (HitResult && GravityFieldGenerator.StopOnActor)
		{
			UE_LOG(LogTemp,Warning, TEXT("The Component Being Hit is: %f"), OutHit.Distance);
			SetActorScale3D(FVector(1,(OutHit.Distance / 70),1));
			SetActorLocation(FVector(GetActorLocation().X,GetActorLocation().Y - 39 * (OutHit.Distance / 70), GetActorLocation().Z),false);
		} else
		{
			SetActorScale3D(FVector(1,(GravityFieldGenerator.MaxDistance / 70),1));
			SetActorLocation(FVector(GetActorLocation().X,GetActorLocation().Y - 39 * (GravityFieldGenerator.MaxDistance / 70), 
			GetActorLocation().Z),false);
		}	
	break;
	case EGravityDirection::Negative_X:
		SetPivotOffset(FVector(-39,0,0));
		
		DrawDebugLine(GetWorld(),GetActorLocation(),(-GetActorForwardVector() * 
		GravityFieldGenerator.MaxDistance )+ GetActorLocation(),FColor::Blue,false,5,0,10);

		HitResult = GetWorld()->LineTraceSingleByChannel(OutHit, GetActorLocation(),(-GetActorForwardVector() * 
		GravityFieldGenerator.MaxDistance )+ GetActorLocation(),ECC_Visibility,CollisionParams);
		
		if (HitResult && GravityFieldGenerator.StopOnActor)
		{
			UE_LOG(LogTemp,Warning, TEXT("The Component Being Hit is: %f"), OutHit.Distance);
			SetActorScale3D(FVector((OutHit.Distance / 70) + 2,1,1));
			SetActorLocation(FVector(GetActorLocation().X -39 * (OutHit.Distance / 70),GetActorLocation().Y , GetActorLocation().Z),false);
		} else
		{
			SetActorScale3D(FVector((GravityFieldGenerator.MaxDistance / 70) + 2,1,1));
			SetActorLocation(FVector(GetActorLocation().X - 39 * (GravityFieldGenerator.MaxDistance/ 70),GetActorLocation().Y , GetActorLocation().Z),false);
		}		 
		break;
	case EGravityDirection::Positive_Y:
		SetPivotOffset(FVector(0,0,-39));
		
		DrawDebugLine(GetWorld(),GetActorLocation(),(GetActorRightVector() * 
		GravityFieldGenerator.MaxDistance )+ GetActorLocation(),FColor::Blue,false,1,0,5);

		HitResult = GetWorld()->LineTraceSingleByChannel(OutHit,GetActorLocation(),(GetActorRightVector() * 
		GravityFieldGenerator.MaxDistance )+ GetActorLocation(),ECC_PhysicsBody,CollisionParams);
		
		if (HitResult && GravityFieldGenerator.StopOnActor)
		{
			UE_LOG(LogTemp,Warning,TEXT("pog"));
			UE_LOG(LogTemp,Warning, TEXT("The Component Being Hit is: %f"), OutHit.Distance);
			SetActorScale3D(FVector(1,(OutHit.Distance / 70),1));
			
			SetActorLocation(FVector(GetActorLocation().X,GetActorLocation().Y + 36 * (OutHit
			.Distance / 70), GetActorLocation().Z),false);
		} else
		{
			SetActorScale3D(FVector(1,(GravityFieldGenerator.MaxDistance / 70),1));
			SetActorLocation(FVector(GetActorLocation().X,GetActorLocation().Y + 36 * (GravityFieldGenerator.MaxDistance / 70), 
			GetActorLocation().Z),false);
		}		 
		break;
	
	case EGravityDirection::Positive_Z:

		SetPivotOffset(FVector(0,0,-39));
		
		DrawDebugLine(GetWorld(),GetActorLocation(),(GetActorUpVector() * 
		GravityFieldGenerator.MaxDistance )+ GetActorLocation(),FColor::Blue,false,5,0,5);

		HitResult = GetWorld()->LineTraceSingleByChannel(OutHit,GetActorLocation(),(GetActorUpVector() * 
		GravityFieldGenerator.MaxDistance )+ GetActorLocation(),ECC_PhysicsBody,CollisionParams);
		
		if (HitResult && GravityFieldGenerator.StopOnActor)
		{
			UE_LOG(LogTemp,Warning, TEXT("The Component Being Hit is: %f"), OutHit.Distance);
			SetActorScale3D(FVector(1,1,(OutHit.Distance / 70)));
			SetActorLocation(FVector(GetActorLocation().X,GetActorLocation().Y,GetActorLocation().Z 
			+ 39 * (OutHit.Distance / 70)),false);
		} else
		{
			SetActorScale3D(FVector(1,1,GravityFieldGenerator.MaxDistance / 70));
			SetActorLocation(FVector(GetActorLocation().X,GetActorLocation().Y,GetActorLocation().Z 
			+ 39 * (GravityFieldGenerator.MaxDistance / 70)),false);
			UE_LOG(LogTemp,Warning,TEXT("NOTPOG"));
		}		 
		break;
	case EGravityDirection::Negative_Z:

		SetPivotOffset(FVector(0,0,-39));

		DrawDebugLine(GetWorld(),GetActorLocation(),(-GetActorUpVector() * 
		GravityFieldGenerator.MaxDistance ) - GetActorLocation(),FColor::Blue,false,1,0,5);

		HitResult = GetWorld()->LineTraceSingleByChannel(OutHit,GetActorLocation(),(-GetActorUpVector() * 
		GravityFieldGenerator.MaxDistance ) - GetActorLocation(),ECC_PhysicsBody,CollisionParams);
		
		if (HitResult && GravityFieldGenerator.StopOnActor)
		{
			UE_LOG(LogTemp,Warning,TEXT("pog"));
			UE_LOG(LogTemp,Warning, TEXT("The Component Being Hit is: %f"), OutHit.Distance);
			SetActorScale3D(FVector(1,1,(OutHit.Distance / 100)));
			SetActorLocation(FVector(GetActorLocation().X,GetActorLocation().Y,GetActorLocation().Z 
			- 28 * (OutHit.Distance / 100)),false);
		} else
		{
			SetActorScale3D(FVector(1,1,GravityFieldGenerator.MaxDistance / 100));
			SetActorLocation(FVector(GetActorLocation().X,GetActorLocation().Y,GetActorLocation().Z 
			- 28 * (GravityFieldGenerator.MaxDistance / 100)),false);
		}		 
		break;
	default: ;
	}

	ActorsAllowedAtOnce = GravityFieldGenerator.AllowedActorsAtOnce;
	GravityType = GravityFieldGenerator.GravityFieldType;
	GravityStrength = GravityFieldGenerator.GravityStrength;
	

}

void AGravityField::DeterminePushForPlayer(APlayerCharacter* character)
{
	switch(GravityType)
	{
	case EGravityFieldType::VERTICAL_PUSH:
			character->PushPlayer(FVector(0,0, GravityStrength),true);
		break;
	case EGravityFieldType::VERTICAL_PULL:
		character->PushPlayer(FVector(0,0, -GravityStrength),true);
		break;
	case EGravityFieldType::HORIZONTAL_PUSH_X:
		character->PushPlayer(FVector(GravityStrength,0, 0),true);
		break;
	case EGravityFieldType::HORIZONTAL_PULL_X:
		character->PushPlayer(FVector(-GravityStrength,0, 0),true);
		break;
	case EGravityFieldType::HORIZONTAL_PUSH_Y:
		character->PushPlayer(FVector(0,GravityStrength, 0),true);
		break;
	case EGravityFieldType::HORIZONTAL_PULL_Y:
		character->PushPlayer(FVector(0,-GravityStrength, 0),true);
		break;
	default: ;
	}
	
}

void AGravityField::DetermineLerpPositionForActor(AActor* actor,ULerp* actorLerpComponent)
{

	UStaticMeshComponent* MeshComp = Cast<UStaticMeshComponent>(actor->GetComponentByClass(UStaticMeshComponent::StaticClass()));
	
	switch(GravityType)
	{
	case EGravityFieldType::VERTICAL_PUSH:
		actorLerpComponent->LerpFVector(actor,actor->GetActorLocation(),FVector(GetActorLocation().X,GetActorLocation().Y,actor->GetActorLocation().Z),0.2);
		MeshComp->AddImpulse(FVector(0,0, 40),NAME_None,true);
		//character->PushPlayer(FVector(0,0, 40),true);
		break;
	case EGravityFieldType::VERTICAL_PULL:
		actorLerpComponent->LerpFVector(actor,actor->GetActorLocation(),FVector(GetActorLocation().X,GetActorLocation().Y,actor->GetActorLocation().Z),0.2);
		MeshComp->AddImpulse(FVector(0,0, -40),NAME_None,true);
		//character->PushPlayer(FVector(0,0, -40),true);
		break;
	case EGravityFieldType::HORIZONTAL_PUSH_X:
		actorLerpComponent->LerpFVector(actor,actor->GetActorLocation(),FVector(actor->GetActorLocation().X,GetActorLocation().Y,GetActorLocation().Z),0.2);
		//character->PushPlayer(FVector(40,0, 0),true);
		MeshComp->AddImpulse(FVector(40,0, 0),NAME_None,true);
		break;
	case EGravityFieldType::HORIZONTAL_PULL_X:
		actorLerpComponent->LerpFVector(actor,actor->GetActorLocation(),FVector(actor->GetActorLocation().X,GetActorLocation().Y,GetActorLocation().Z),0.2);
		//character->PushPlayer(FVector(-40,0, 40),true);.
		MeshComp->AddImpulse(FVector(-40,0, 0),NAME_None,true);
		break;
	case EGravityFieldType::HORIZONTAL_PUSH_Y:
		actorLerpComponent->LerpFVector(actor,actor->GetActorLocation(),FVector(GetActorLocation().X,actor->GetActorLocation().Y,GetActorLocation().Z),0.2);
		//character->PushPlayer(FVector(0,40, 40),true);
		UE_LOG(LogTemp,Warning,TEXT("HORI"));
		MeshComp->AddImpulse(FVector(0,40, 0),NAME_None,true);
		break;
	case EGravityFieldType::HORIZONTAL_PULL_Y:
		actorLerpComponent->LerpFVector(actor,actor->GetActorLocation(),FVector(GetActorLocation().X,actor->GetActorLocation().Y,GetActorLocation().Z),0.2);
		//character->PushPlayer(FVector(0,-40, 40),true);
		MeshComp->AddImpulse(FVector(0,-40, 0),NAME_None,true);
		break;
	default: ;
	}
}


void AGravityField::DetermineLerpPositionForPlayer(APlayerCharacter* character)
{
	switch(GravityType)
	{
	case EGravityFieldType::VERTICAL_PUSH:
		character->LerpComponent->LerpFVector(character,character->GetActorLocation(),FVector(GetActorLocation().X,GetActorLocation().Y,character->GetActorLocation().Z),0.2);
		character->PushPlayer(FVector(0,0, 40),true);
		break;
	case EGravityFieldType::VERTICAL_PULL:
		character->LerpComponent->LerpFVector(character,character->GetActorLocation(),FVector(GetActorLocation().X,GetActorLocation().Y,character->GetActorLocation().Z),0.2);
		character->PushPlayer(FVector(0,0, -40),true);
		break;
	case EGravityFieldType::HORIZONTAL_PUSH_X:
		character->LerpComponent->LerpFVector(character,character->GetActorLocation(),FVector(character->GetActorLocation().X,GetActorLocation().Y,GetActorLocation().Z),0.2);
		character->PushPlayer(FVector(40,0, 0),true);
		break;
	case EGravityFieldType::HORIZONTAL_PULL_X:
		character->LerpComponent->LerpFVector(character,character->GetActorLocation(),FVector(character->GetActorLocation().X,GetActorLocation().Y,GetActorLocation().Z),0.2);
		character->PushPlayer(FVector(-40,0, 40),true);
		break;
	case EGravityFieldType::HORIZONTAL_PUSH_Y:
		 character->LerpComponent->LerpFVector(character,character->GetActorLocation(),FVector(GetActorLocation().X,character->GetActorLocation().Y,GetActorLocation().Z),0.2);
		character->PushPlayer(FVector(0,40, 0),true);
		break;
	case EGravityFieldType::HORIZONTAL_PULL_Y:
		character->LerpComponent->LerpFVector(character,character->GetActorLocation(),FVector(GetActorLocation().X,character->GetActorLocation().Y,GetActorLocation().Z),0.2);
		character->PushPlayer(FVector(0,-40, 0),true);
		break;
	default: ;
	}
}

void AGravityField::DeterminePushForActor(UStaticMeshComponent* MeshComp)
{
	switch(GravityType)
	{
	case EGravityFieldType::VERTICAL_PUSH:
		MeshComp->AddImpulse(FVector(0,0, GravityStrength),NAME_None,true);
		break;
	case EGravityFieldType::VERTICAL_PULL:
		MeshComp->AddImpulse(FVector(0,0, -GravityStrength),NAME_None,true);
		break;
	case EGravityFieldType::HORIZONTAL_PUSH_X:
		MeshComp->AddImpulse(FVector(GravityStrength,0, 0),NAME_None,true);
		break;
	case EGravityFieldType::HORIZONTAL_PULL_X:
		MeshComp->AddImpulse(FVector(-GravityStrength,0, 0),NAME_None,true);
		break;
	case EGravityFieldType::HORIZONTAL_PUSH_Y:
		MeshComp->AddImpulse(FVector(0,GravityStrength, 0),NAME_None,true);
		break;
	case EGravityFieldType::HORIZONTAL_PULL_Y:
		MeshComp->AddImpulse(FVector(0,-GravityStrength, 0),NAME_None,true);
		break;
	default: ;
	}
}


// void AGravityField::OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
// 	UPrimitiveComponent* OtherComponent, int32 OtherBodyIndex, bool bFromSweep,
// 	const FHitResult& SweepResult)
// {
// 	UE_LOG(LogTemp,Warning,TEXT("OVERLAPPED"));
//
// 	if (Cast<APlayerCharacter>(OtherActor))
// 	{
// 		APlayerCharacter* character = Cast<APlayerCharacter>(OtherActor);
// 		character->SetGravity(0.1);
// 	}
// 	
// 	OnEnter(OtherActor);
// }
//
// void AGravityField::OnOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
// 	UPrimitiveComponent* OtherComponent, int32 OtherBodyIndex)
// {
// 	UE_LOG(LogTemp,Warning,TEXT("OVERLAPPEDended"));
// 	if (Cast<APlayerCharacter>(OtherActor))
// 	{
// 		APlayerCharacter* character = Cast<APlayerCharacter>(OtherActor);
// 		character->SetGravity(0.1);
// 	}	//OnEnter(OtherActor);
// }

