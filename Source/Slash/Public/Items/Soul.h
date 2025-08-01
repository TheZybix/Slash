// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Items/Item.h"
#include "Soul.generated.h"

/**
 * 
 */
UCLASS()
class SLASH_API ASoul : public AItem
{
	GENERATED_BODY()

public:
	virtual void Tick(float DeltaTime) override;

protected:
	virtual void BeginPlay() override;
	/* Child class of item, that overrides OnSphereOverlap to give souls to player */
	virtual void OnSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult) override;

private:
	/* Set amount of souls to be given */
	UPROPERTY(EditAnywhere, Category = "Soul Properties")
	int32 Souls;

	/* Set the height offset from the ground for the soul pickup */
	double DesiredZ;

	/* Determine how fast it should be sinking on spawn */
	UPROPERTY(EditAnywhere, Category = "Soul Properties")
	float DriftRate = -15.f;

public:
	FORCEINLINE int32 GetSouls() const { return Souls; }
	FORCEINLINE void SetSouls(int32 NumberOfSouls) { Souls = NumberOfSouls; }
};
