// The attribute component provides actors with basic stats such as health, stamina, gold and souls
// With variables exposed to blueprints, balancing blueprint actors, even instances of them, can be done quickly

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "AttributeComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class SLASH_API UAttributeComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	UAttributeComponent();
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	void RegenStamina(float DeltatTime);

protected:
	virtual void BeginPlay() override;

private:

	// All stats the character will have access to
	
	//Current Health of the actor
	UPROPERTY(EditAnywhere, Category = "Actor Attributes")
	float Health;

	UPROPERTY(EditAnywhere, Category = "Actor Attributes")
	float MaxHealth;

	//Current stamina of the actor
	UPROPERTY(EditAnywhere, Category = "Actor Attributes")
	float Stamina;

	UPROPERTY(EditAnywhere, Category = "Actor Attributes")
	float MaxStamina;
	
	UPROPERTY(EditAnywhere, Category = "Actor Attributes")
	float DodgeCost = 25.f;

	UPROPERTY(VisibleAnywhere, Category = "Actor Attributes")
	float AttackCost;

	UPROPERTY(EditAnywhere, Category = "Actor Attributes")
	int32 Gold;

	UPROPERTY(EditAnywhere, Category = "Actor Attributes")
	int32 Souls;

	UPROPERTY(EditAnywhere, Category = "Actor Attributes")
	float StaminaRegenRate = 12.f;

public:
	// All functions modifying the stats of the character
	void ReceiveDamage(float Damage);
	void ReceiveHealing(float Healing);
	float GetHealthPercent();
	void UseStamina (float StaminaCost);
	float GetStaminaPercent();
	bool IsAlive();
	void AddSouls (int32 NumberOfSouls);
	void AddGold (int32 GoldAmount);
	void SetWeaponStaminaCost(float StaminaCost);

	// Efficient functions to retrieve information from this component
	FORCEINLINE int32 GetGold() const { return Gold; }
	FORCEINLINE int32 GetSouls() const { return Souls; }
	FORCEINLINE float GetDodgeCost() const { return DodgeCost; }
	FORCEINLINE float GetAttackCost() const { return AttackCost; }
};

