// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Items/Item.h"
#include "HealthPotion.generated.h"

/**
 * 
 */
UCLASS()
class SLASH_API AHealthPotion : public AItem
{
	GENERATED_BODY()
	
protected:
	/* Child class of item, that overrides OnSphereOverlap to restore health to player */
	virtual void OnSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult) override;

private:
	/* Set amount of health to be restored */
	UPROPERTY(EditAnywhere, Category = "HealthPotion Properties")
	float Health = 25.f;

public:
	FORCEINLINE float GetHealth() const { return Health; }
	FORCEINLINE void SetHealth(float AmountOfHealth) { Health = AmountOfHealth; }
};
