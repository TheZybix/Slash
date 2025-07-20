// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Interfaces/HitInterface.h"
#include "BreakableActor.generated.h"

class UGeometryCollectionComponent;
class UNavModifierComponent;

UCLASS()
class SLASH_API ABreakableActor : public AActor, public IHitInterface
{
	GENERATED_BODY()
	
public:	
	ABreakableActor();

	/**<IHitInterface>*/
	virtual void GetHit_Implementation(const FVector& ImpactPoint, AActor* Hitter) override;
	/**</IHitInterface>*/

protected:
	virtual void BeginPlay() override;

	/* Spawn treasure when hit */
	void SpawnTreasure();

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	UGeometryCollectionComponent* GeometryCollection;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	class UCapsuleComponent* Capsule;
	
private:
	/* Set an array of Items the treasure can spawn */
	UPROPERTY(EditAnywhere, Category = "Breakable Properties")
	TArray<TSubclassOf<class AItem>> TreasureClasses;

	/* Treasure blocks the nav mesh */
	UPROPERTY(VisibleInstanceOnly)
	TObjectPtr<UNavModifierComponent> NavBlocker;
		
	bool bBroken = false;
};
