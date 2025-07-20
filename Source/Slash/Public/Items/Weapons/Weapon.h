	FVector VaultOrigin;
FVector VaultStartPosition;
FVector VaultMiddlePosition;
FVector VaultEndPosition;
bool CanVault;// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Characters/CharacterTypes.h"
#include "Items/Item.h"
#include "Weapon.generated.h"

enum class EWeaponType : uint8;
class USoundBase;
class UBoxComponent;

/**
 * 
 */
UCLASS()
class SLASH_API AWeapon : public AItem
{
	GENERATED_BODY()
	
public:
	AWeapon();
	/* Attach weapon to character functionality */
	void AttachMeshToSocket(USceneComponent* InParent, FName InSocketName);
	void PlayEquipSound();
	void DisableSphereCollision();
	void DeactivateEmbers();
	void Equip(USceneComponent* InParent, FName InSocketName, AActor* NewOwner, APawn* NewInstigator);

	/* Hit characters functionality */
	void ExecuteGetHit(FHitResult BoxHit);
	bool ActorIsSameType(AActor* OtherActor);

protected:
	virtual void BeginPlay() override;
	
	UFUNCTION()
	void OnBoxOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult);

	/* Create field for breakable objects */
	UFUNCTION(BlueprintImplementableEvent)
	void CreateFields(const FVector& FieldLocation);

	/* Set the weapon type in the blueprint child */
	UPROPERTY(EditAnywhere)
	EWeaponType WeaponType = EWeaponType::EWT_SwordOneHanded;

	/* Set the stamina cost, heavier and larger weapons should consume more stamina per attack */
	UPROPERTY(EditDefaultsOnly)
	float StaminaCost;

private:
	/* Box trace functionality */
	void BoxTrace(FHitResult& BoxHit);

	UPROPERTY(EditAnywhere, Category = "WeaponProperties")
	FVector BoxTraceExtent = FVector(5.f);

	UPROPERTY(EditAnywhere, Category = "WeaponProperties")
	bool bShowBoxDebug = false;
	
	UPROPERTY(VisibleAnywhere, Category = "WeaponProperties")
	UBoxComponent* WeaponBox;

	UPROPERTY(VisibleAnywhere)
	USceneComponent* BoxTraceStart;

	UPROPERTY(VisibleAnywhere)
	USceneComponent* BoxTraceEnd;

	/* Weapon properties */
	UPROPERTY(EditAnywhere, Category = "WeaponProperties")
	USoundBase* EquipSound;

	UPROPERTY(EditAnywhere, Category = "WeaponProperties")
	float Damage = 20.f;

public:
	FORCEINLINE UBoxComponent* GetWeaponBox() const { return WeaponBox;}
	TArray<AActor*> IgnoreActors;
};
