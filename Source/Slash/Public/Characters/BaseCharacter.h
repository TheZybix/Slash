// The BaseCharacter is the basis for both the enemy and player characters, allowing them to use weapons, get hit and prepare animation montages among other things
// Should the player and enemy need to transfer functionality, it can easily be placed here so they have access

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Interfaces/HitInterface.h"
#include "CharacterTypes.h"
#include "Interfaces/VaultInterface.h"
#include "Interfaces/WeaponInterface.h"
#include "BaseCharacter.generated.h"

class AWeapon;
class UAttributeComponent;
class UMotionWarpingComponent;
class UAnimMontage;

UCLASS()
class SLASH_API ABaseCharacter : public ACharacter, public IHitInterface, public IWeaponInterface, public IVaultInterface
{
	GENERATED_BODY()

public:
	ABaseCharacter();
	virtual void Tick(float DeltaTime) override;
	
	/**<IWeaponTypeInterface>*/
	virtual void SetWeaponProperties(EWeaponType WeaponType, float StaminaCost) override;
	/**</IWeaponTypeInterface>*/

	/**<IVaultInterface*/
	virtual void SetVaultWarpLocations(const FVector Origin, const FVector Start, const FVector Middle, const FVector End) override;
	virtual void CanVault(const bool bVault) override;
	virtual bool CanUpdateWarpLocations() override;
	/**</IVaultInterface*/

protected:
	virtual void BeginPlay() override;

	virtual void Attack();
	virtual void Vault();
	

	
	/* Character gets hit */
	virtual void GetHit_Implementation(const FVector& ImpactPoint, AActor* Hitter) override;
	void DirectionalHitReact(const FVector& ImpactPoint);
	virtual void HandleDamage(float DamageAmount);
	void PlayHitSound(const FVector& ImpactPoint);
	void SpawnHitParticles(const FVector& ImpactPoint);
	void DisableCapsule();

	UFUNCTION(BlueprintNativeEvent)
	void Die();
	
	virtual bool CanAttack();
	bool IsAlive();
	void DisableMeshCollision();


	/* Montage Functions */
	void PlayHitReactMontage(const FName& SectionName);
	virtual int32 PlayAttackMontage();
	virtual int32 PlayDeathMontage();
	virtual void PlayDodgeMontage();
	virtual void PlayVaultMontage();
	void StopAttackMontage();


	/* Motion Warp Functions */
	UFUNCTION(BlueprintCallable)
	FVector GetTranslationWarpTarget();

	UFUNCTION(BlueprintCallable)
	FVector GetRotationWarpTarget();

	virtual void SetCombatMotionWarp();

	UFUNCTION(BlueprintCallable)
	void SetWeaponCollisionEnabled(ECollisionEnabled::Type CollisionEnabled);

	/* Finish Actions */
	UFUNCTION(BlueprintCallable)
	virtual void AttackEnd();

	UFUNCTION(BlueprintCallable)
	virtual void DodgeEnd();

	UFUNCTION(BlueprintCallable)
	virtual void FinishVault();

	/* Blueprint implemented variables */
	UPROPERTY(VisibleAnywhere, Category = "Weapon")
	AWeapon* EquippedWeapon;
	
	UPROPERTY(VisibleAnywhere)
	UAttributeComponent* Attributes;

	UPROPERTY(BlueprintReadWrite)
	UMotionWarpingComponent* MotionWarpingComponent;

	UPROPERTY(BlueprintReadOnly, Category= "Combat")
	AActor* CombatTarget;

	UPROPERTY(EditAnywhere, Category = "Combat")
	double WarpTargetDistance = 75.f;

	UPROPERTY(BlueprintReadWrite)
	TEnumAsByte<EDeathPose> DeathPose;

	bool bCanVault = false;
	float StaminaAttackCost;

private:
	void PlayMontageSection(UAnimMontage* Montage, const FName& SectionName);
	int32 PlayRandomMontageSection(UAnimMontage* Montage, const TArray<FName>& SectionNames);

	/* Hit Effects */
	UPROPERTY(EditAnywhere, Category = Combat)
	USoundBase* HitSound;

	UPROPERTY(EditAnywhere, Category = Combat)
	UParticleSystem* HitParticle;
	
	/* Montages */
	UPROPERTY(VisibleDefaultsOnly, Category = Combat)
	UAnimMontage* ActiveAttackMontage;

	UPROPERTY(EditDefaultsOnly, Category = Combat)
	UAnimMontage* SwordOneHandedAttackMontage;

	UPROPERTY(EditDefaultsOnly, Category = Combat)
	UAnimMontage* AxeOneHandedAttackMontage;

	UPROPERTY(EditDefaultsOnly, Category = Combat)
	UAnimMontage* DodgeMontage;

	UPROPERTY(EditDefaultsOnly, Category = Combat)
	UAnimMontage* VaultMontage;

	UPROPERTY(EditDefaultsOnly, Category = Combat)
	UAnimMontage* HitReactMontage;

	UPROPERTY(EditDefaultsOnly, Category = Combat)
	UAnimMontage* DeathMontage;

	/* Montage Section Arrays */
	UPROPERTY(VisibleDefaultsOnly, Category = Combat)
	TArray<FName> AttackMontageSections;

	UPROPERTY(EditAnywhere, Category = Combat)
	TArray<FName> AttackMontageSectionsSwordOneHanded;

	UPROPERTY(EditAnywhere, Category = Combat)
	TArray<FName> AttackMontageSectionsAxeOneHanded;

	UPROPERTY(EditAnywhere, Category = Combat)
	TArray<FName> DeathMontageSections;

	/* Motion Warping */
	FVector VaultOrigin;
	FVector VaultStartPosition;
	FVector VaultMiddlePosition;
	FVector VaultEndPosition;

public:
	/* Use FORCEINLINE because it is called frequently in the Animation Blueprint Threadsafe Function to update DeathPose */
	FORCEINLINE TEnumAsByte<EDeathPose> GetDeathPose() const { return DeathPose; }
};

