// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "WeaponInterface.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UWeaponInterface : public UInterface
{
	GENERATED_BODY()
};

class SLASH_API IWeaponInterface
{
	GENERATED_BODY()

public:
	/* Set the properties of this weapon in the actor picking it up, including stamina cost */
	virtual void SetWeaponProperties(EWeaponType WeaponType, float WeaponStaminaCost);
};
