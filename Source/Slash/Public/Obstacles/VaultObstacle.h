// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "VaultObstacle.generated.h"

class UNavModifierComponent;
class UBoxComponent;

UCLASS()
class SLASH_API AVaultObstacle : public AActor
{
	GENERATED_BODY()
	
public:	
	AVaultObstacle();

	/* Transferred Motion Warp Locations */
	FVector Origin;
	FVector StartPosition;
	FVector MiddlePosition;
	FVector EndPosition;
	
	bool AllowVault;

protected:
	virtual void BeginPlay() override;
	void SetTargetWarpTargets(AActor* OtherActor);
	void DisableVault(AActor* OtherActor);

	/* Generate overlap events for collision boxes on both sides of the vault obstacle */
	UFUNCTION()
	virtual void OnFrontBoxOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult);

	UFUNCTION()
	virtual void OnFrontBoxEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	UFUNCTION()
	virtual void OnBackBoxOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult);

	UFUNCTION()
	virtual void OnBackBoxEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

private:

	virtual void SetMotionTargets(FVector Start, FVector Front, FVector Middle, FVector End);

	/* Item mesh to visually represent the obstacle and for characters to collide with */
	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<UStaticMeshComponent> ItemMesh;

	/* Create two box collisions, so the actor knows if the character is trying to vault from the front or the back
	 * Using Box Collisions instead of raycasting over obstacles makes this vaulting less dynamic but more controllable for linear level design, as was my intention here*/
	UPROPERTY(VisibleInstanceOnly)
	TObjectPtr<UBoxComponent> FrontBoxCollision;

	UPROPERTY(VisibleInstanceOnly)
	TObjectPtr<UBoxComponent> BackBoxCollision;

	/* Add nav blocker so enemies won't attempt to walk through the mesh */
	UPROPERTY(VisibleInstanceOnly)
	TObjectPtr<UNavModifierComponent> NavBlocker;

	/* Vault Motion Warp Targets
	 * The Warp Targets can easily be positioned in the blueprint deriving from this class
		This allows the creation of multiple children with different static meshes, heights and vault distances and only requires repositioning of the SceneComponents for the vault to match*/
	UPROPERTY(VisibleInstanceOnly)
	TObjectPtr<USceneComponent> WorldOriginFront;
	
	UPROPERTY(VisibleInstanceOnly)
	TObjectPtr<USceneComponent> WorldOriginBack;
	
	UPROPERTY(VisibleInstanceOnly)
	TObjectPtr<USceneComponent> WorldVaultFront;
	
	UPROPERTY(VisibleInstanceOnly)
	TObjectPtr<USceneComponent> WorldVaultMiddle;
	
	UPROPERTY(VisibleInstanceOnly)
	TObjectPtr<USceneComponent> WorldVaultBack;
};
