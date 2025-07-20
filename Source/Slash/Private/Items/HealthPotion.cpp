// Fill out your copyright notice in the Description page of Project Settings.


#include "Items/HealthPotion.h"

#include "Interfaces/PickupInterface.h"

void AHealthPotion::OnSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
                                    UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	Super::OnSphereOverlap(OverlappedComponent, OtherActor, OtherComp, OtherBodyIndex, bFromSweep, SweepResult);

	if (OtherActor && !OtherActor->ActorHasTag("Player")) return; // Ensure only the player can interact with the health potion

	/* Restore health to the player on sphere overlap */
	IPickupInterface* PickUpInterface = Cast<IPickupInterface>(OtherActor);
	if (PickUpInterface)
	{
		PickUpInterface->AddHealth(this);
		
		SpawnPickupSystem();
		SpawnPickupSound();
		Destroy();
	}
}
