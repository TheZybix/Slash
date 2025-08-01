// Fill out your copyright notice in the Description page of Project Settings.


#include "Breakable/BreakableActor.h"

#include "NavModifierComponent.h"
#include "Components/CapsuleComponent.h"
#include "GeometryCollection//GeometryCollectionComponent.h"
#include "Items/Treasure.h"


ABreakableActor::ABreakableActor()
{
	PrimaryActorTick.bCanEverTick = false;

	GeometryCollection = CreateDefaultSubobject<UGeometryCollectionComponent>(TEXT("GeometryCollection"));
	SetRootComponent(GeometryCollection);
	GeometryCollection->SetGenerateOverlapEvents(true);
	GeometryCollection->SetCollisionResponseToChannel(ECC_Destructible, ECR_Ignore);
	GeometryCollection->SetCollisionResponseToChannel(ECC_Camera, ECR_Ignore);
	GeometryCollection->SetCollisionResponseToChannel(ECC_Pawn, ECR_Ignore);

	Capsule = CreateDefaultSubobject<UCapsuleComponent>(TEXT("Capsule"));
	Capsule->SetupAttachment(GetRootComponent());
	Capsule->SetCollisionResponseToAllChannels(ECR_Ignore);
	Capsule->SetCollisionResponseToChannel(ECC_Pawn, ECR_Block);

	NavBlocker = CreateDefaultSubobject<UNavModifierComponent>(TEXT("NavBlocker"));
}


void ABreakableActor::BeginPlay()
{
	Super::BeginPlay();
}

void ABreakableActor::GetHit_Implementation(const FVector& ImpactPoint, AActor* Hitter)
{
	if (bBroken) return; 
	bBroken = true; //Prevent BreakableActor from breaking more than once, for example by colling with other BreakableActors

	Capsule->SetCollisionResponseToChannel(ECC_Pawn, ECR_Ignore);
	SpawnTreasure();
}

void ABreakableActor::SpawnTreasure()
{
	UWorld* World = GetWorld();
	if (World && TreasureClasses.Num() > 0) //Check if there is treasure to spawn or if the treasure array is empty
	{
		FVector Location = GetActorLocation();
		Location.Z+= 75.f;
		int32 Selection = FMath::RandRange(0, TreasureClasses.Num() - 1);
		World->SpawnActor<AItem>(TreasureClasses[Selection],Location, GetActorRotation());
		//Spawn random treasure from the treasure array, slightly elevated above the ground
	}
}
