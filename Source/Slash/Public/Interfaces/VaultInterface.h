// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "VaultInterface.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UVaultInterface : public UInterface
{
	GENERATED_BODY()
};

class SLASH_API IVaultInterface
{
	GENERATED_BODY()

public:
	/* Use this interface to update the vault warp capabilities of all actors who implement it */
	virtual void SetVaultWarpLocations(const FVector Origin, const FVector Start, const FVector Middle, const FVector End);
	virtual void CanVault(const bool bVault);
	virtual bool CanUpdateWarpLocations();
};
