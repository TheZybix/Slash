// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BaseCharacter.h"
#include "InputActionValue.h"
#include "CharacterTypes.h"
#include "Interfaces/PickupInterface.h"
#include "Interfaces/WeaponInterface.h"
#include "SlashCharacter.generated.h"

class UVictoryScreen;
class UDeathScreen;
class USlashOverlay;
class UInputMappingContext;
class UInputAction;
class USpringArmComponent;
class UCameraComponent;
class UGroomComponent;
class AItem;
class UAnimMontage;
class USphereComponent;

UCLASS()
class SLASH_API ASlashCharacter : public ABaseCharacter, public IPickupInterface
{
	GENERATED_BODY()

public:
	ASlashCharacter();

	/* This actor is a child of the BaseCharacter */
	virtual void Tick(float DeltaTime) override;
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser) override;
	virtual void Jump() override;

	/**<IHitInterface>*/
	virtual void GetHit_Implementation(const FVector& ImpactPoint, AActor* Hitter) override;
	/**</IHitInterface>*/
	
	/**<IPickupInterface>*/
	virtual void SetOverlappingItem(AItem* Item) override;
	virtual void AddSouls(ASoul* Soul) override;
	virtual void AddGold(ATreasure* Treasure) override;
	virtual void AddHealth(AHealthPotion* Health) override;
	/**</IPickupInterface>*/
	
	bool bCanWin = false;
	
protected:
	virtual void BeginPlay() override;

	/* Callbacks for input*/
	UPROPERTY(EditAnywhere, Category = Input)
	UInputMappingContext* SlashContext;

	UPROPERTY(EditAnywhere, Category = Input)
	UInputAction* MovementAction;

	UPROPERTY(EditAnywhere, Category = Input)
	UInputAction* LookAction;

	UPROPERTY(EditAnywhere, Category = Input)
	UInputAction* JumpAction;

	UPROPERTY(EditAnywhere, Category = Input)
	UInputAction* EKeyAction;

	UPROPERTY(EditAnywhere, Category = Input)
	UInputAction* AttackAction;

	UPROPERTY(EditAnywhere, Category = Input)
	UInputAction* DodgeAction;
	
	UPROPERTY(EditAnywhere, Category = Input)
	UInputAction* InteractAction;

	void Move(const FInputActionValue& Value);
	void Look(const FInputActionValue& Value);
	void EKeyPressed();
	virtual void Attack() override;
	void Dodge();
	void Interact();
	void Win();

	/* Combat */
	void EquipWeapon(AWeapon* Weapon);
	virtual void AttackEnd() override;
	virtual void DodgeEnd() override;
	virtual bool CanAttack() override;
	bool CanDisarm();
	bool CanArm();
	void Disarm();
	void Arm();
	void PlayEquipMontage(const FName& SectionName);
	void PlayStandUpMontage();
	void DeathScreenUI();
	void VictoryScreenUI();
	virtual void Die_Implementation() override;


	/* Targeting */
	void SetCombatTarget(AActor* Target);
	void ClearCombatTarget();
	void GetTargetsInRange(TArray<AActor*>& Targets);
	void FindCombatTarget();
	AActor* ClosestCombatTarget(TArray<AActor*>& Targets);

	/* Vaulting */
	virtual void Vault() override;
	virtual void FinishVault() override;

	UPROPERTY(EditAnywhere, Category = "Targeting")
	TSubclassOf<AActor> TargetClassFilter;

	/* Attaching */
	UFUNCTION(BlueprintCallable)
	void AttachWeaponToBack();

	UFUNCTION(BlueprintCallable)
	void AttachWeaponToHand();

	UFUNCTION(BlueprintCallable)
	void FinishEquipping();

	UFUNCTION(BlueprintCallable)
	void HitReactEnd();

	UFUNCTION(BlueprintCallable)
	void DodgeRollInvincibility();

	UFUNCTION(BlueprintCallable)
	void DodgeRollInvincibilityEnd();

	UFUNCTION(BlueprintCallable)
	void StandUpEnd();
	
	UPROPERTY(EditDefaultsOnly, Category = "UI")
	TSubclassOf<UDeathScreen> DeathScreenClass;

	UPROPERTY(EditDefaultsOnly, Category = "UI")
	TSubclassOf<UVictoryScreen> VictoryScreenClass;


	
private:
	void InitializeSlashOverlay(APlayerController* PlayerController);
	void SetHUDHealth();
	void StandUp();

	/* Movement */
	void DetermineDodgeRotation(FVector ForwardDir, FVector RightDir, FVector2D InputValues);
	FRotator DodgeRotation;
	
	ECharacterState CharacterState = ECharacterState::ECS_Unequipped;
	bool IsDodging = false;

	/*CharacterComponents*/
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<USpringArmComponent> CameraBoom;

	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UCameraComponent> Camera;

	UPROPERTY(VisibleAnywhere, Category = Hair)
	TObjectPtr<UGroomComponent> Hair;
	
	UPROPERTY(VisibleAnywhere, Category = Hair)
	TObjectPtr<UGroomComponent> Eyebrows;

	UPROPERTY(VisibleInstanceOnly)
	AItem* OverlappingItem;

	UPROPERTY(VisibleInstanceOnly)
	TObjectPtr<USphereComponent> Sphere;

	UPROPERTY(EditDefaultsOnly, Category = Montages)
	UAnimMontage* EquipMontage;

	UPROPERTY(EditDefaultsOnly, Category = Montages)
	UAnimMontage* StandingUpMontage;

	UPROPERTY(BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	EActionState ActionState = EActionState::EAS_IdleSitting;

	UPROPERTY()
	USlashOverlay* SlashOverlay;

	UPROPERTY()
	UDeathScreen* DeathScreen;

	UPROPERTY()
	UVictoryScreen* VictoryScreen;

	bool bReceiveInput = true;

public:
	// Efficiently receive the characterstate and action state, not using a function call but inlining the functionality directly
	FORCEINLINE ECharacterState GetCharacterState() const{return CharacterState;}
	FORCEINLINE EActionState GetActionState() const{return ActionState;}
};