// Fill out your copyright notice in the Description page of Project Settings.


#include "Items/Soul.h"
#include "Interfaces/PickUpInterface.h"
#include "Kismet/KismetSystemLibrary.h"

void ASoul::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	const double LocationZ = GetActorLocation().Z;

	if (LocationZ > DesiredZ)
	{
		const FVector Location = FVector(0, 0, DriftRate * DeltaTime);
		AddActorWorldOffset(Location);
	}
}

void ASoul::BeginPlay()
{
	Super::BeginPlay();


	/* Perform a line trace from the spawn location straight down to determine a location in the world for the soul to drift down to */
	const FVector Start = GetActorLocation();
	const FVector End = Start - FVector(0, 0, 2000);
	TArray<TEnumAsByte<EObjectTypeQuery>> ObjectTypes;
	ObjectTypes.Add(EObjectTypeQuery::ObjectTypeQuery1);
	TArray<AActor*> ActorsToIgnore;
	ActorsToIgnore.Add(GetOwner());
	FHitResult HitResult;
	
	UKismetSystemLibrary::LineTraceSingleForObjects(this, Start, End, ObjectTypes, false, ActorsToIgnore, EDrawDebugTrace::None, HitResult, true);
	DesiredZ = HitResult.ImpactPoint.Z + 75.f;
}

void ASoul::OnSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
                            UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	IPickupInterface* PickUpInterface = Cast<IPickupInterface>(OtherActor);

	if (OtherActor && !OtherActor->ActorHasTag("Player")) return; // Ensure only the player can interact with the souls pick up

	/* Give souls to the player on sphere overlap */
	if (PickUpInterface)
	{
		PickUpInterface->AddSouls(this);
		
		SpawnPickupSystem();
        SpawnPickupSound();
        Destroy();
	}
}
