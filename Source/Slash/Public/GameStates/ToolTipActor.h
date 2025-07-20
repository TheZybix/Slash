// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ToolTipActor.generated.h"

class UToolTipComponent;
class UBoxComponent;

UCLASS()
class SLASH_API AToolTipActor : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AToolTipActor();
	void SetUpToolTipComponent();

protected:
	virtual void BeginPlay() override;
	
	UFUNCTION()
	virtual void OnBoxOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult);

	UFUNCTION()
	virtual void OnBoxEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	/* Set the custom tooltip text to be displayed in the world */
	UPROPERTY(EditAnywhere)
	FText ToolTipText;

	/* Select the target actor the tooltip should be attached to */
	UPROPERTY(EditAnywhere)
	AActor* ToolTipTargetActor;

	/* Add an offset to the positioning for fine tuning */
	UPROPERTY(EditAnywhere)
	FVector ToolTipOffset;

	/* Determine showing the tooltip when overlapping more than once */
	UPROPERTY(EditAnywhere)
	bool bRemainVisible = false;

	/* Determine if the player can win the game by reaching this tooltip */
	UPROPERTY(EditAnywhere)
	bool bIsVictory = false;

private:
	void ShowTooltip();
	void HideTooltip();
	bool bShowTooltip = true;
	
	UPROPERTY(VisibleInstanceOnly)
	TObjectPtr<UBoxComponent> ToolTipBoxComponent;

	UPROPERTY(VisibleInstanceOnly)
	TObjectPtr<UToolTipComponent> ToolTipComponent;

	FVector ToolTipLocation;
};
