// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Items/Item.h"
#include "Treasure.generated.h"

/**
 * 
 */
UCLASS()
class SLASH_API ATreasure : public AItem
{
	GENERATED_BODY()

protected:
	/* Child class of item, that overrides OnSphereOverlap to give gold to player */
	virtual void OnSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult) override;

private:
	/* Set amount of gold to be given */
	UPROPERTY(EditAnywhere, Category = "Treasure Properties")
	int32 Gold;

public:
	FORCEINLINE int32 GetGold() const { return Gold; }
};
