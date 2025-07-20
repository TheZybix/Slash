// Fill out your copyright notice in the Description page of Project Settings.


#include "Characters/BaseCharacter.h"
#include "Components/BoxComponent.h"
#include "Items/Weapons/Weapon.h"
#include "Kismet/GameplayStatics.h"
#include "Components/AttributeComponent.h"
#include "MotionWarpingComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"


ABaseCharacter::ABaseCharacter()
{
	PrimaryActorTick.bCanEverTick = true;

	Attributes = CreateDefaultSubobject<UAttributeComponent>(TEXT("Attributes"));
	MotionWarpingComponent = CreateDefaultSubobject<UMotionWarpingComponent>(TEXT("MotionWarping"));
	GetCapsuleComponent()->SetCollisionResponseToChannel(ECC_Camera, ECR_Ignore);

}

void ABaseCharacter::BeginPlay()
{
	Super::BeginPlay();
	
}

void ABaseCharacter::GetHit_Implementation(const FVector& ImpactPoint, AActor* Hitter)
{
	/* If the character is alive, play directional hit react, otherwise it dies
		Always plays HitSound and spawns HitParticles as long as they're set */
	if (IsAlive() && Hitter)
	{
		DirectionalHitReact(Hitter->GetActorLocation());
	}
	else Die();
	
	PlayHitSound(ImpactPoint);
	SpawnHitParticles(ImpactPoint);
}

void ABaseCharacter::Attack()
{
	/* If the combat target is tagged as dead, it becomes invalid
		The function is additionally overriden in its child classes */
	if (CombatTarget && CombatTarget->ActorHasTag(FName("Dead")))
	{
		CombatTarget = nullptr;
	}
}

void ABaseCharacter::Vault()
{
	/* If the character can vault, it first sets its position and rotation based on the obstacle origin point, updates its warp motion targets and then vaults over the obstacle
		The movement mode is set to flying for this duration because otherwise the player would glitch through the obstacle rather than vault over it*/
	if (VaultMontage && bCanVault)
	{
		SetActorLocation(VaultOrigin);
		
		FVector Vector = VaultStartPosition - GetActorLocation();
		FRotator VaultRotation = Vector.Rotation();
		VaultRotation.Pitch = 0.f;
		SetActorRotation(VaultRotation);

		MotionWarpingComponent->AddOrUpdateWarpTargetFromLocation("VaultStart", VaultStartPosition);
		MotionWarpingComponent->AddOrUpdateWarpTargetFromLocation("VaultMiddle", VaultMiddlePosition);
		MotionWarpingComponent->AddOrUpdateWarpTargetFromLocation("VaultEnd", VaultEndPosition);

		GetCharacterMovement()->SetMovementMode(MOVE_Flying);
		
		PlayVaultMontage();
		bCanVault = false;
	}
}


bool ABaseCharacter::CanUpdateWarpLocations()
{
	return IVaultInterface::CanUpdateWarpLocations();
}

void ABaseCharacter::Die_Implementation()
{
	/* Add a tag so other actors can check if this actor is dead, then play the death montage */
	Tags.Add(FName("Dead"));
	PlayDeathMontage();
}

void ABaseCharacter::AttackEnd()
{
	/* This function is overriden in its child classes */
}

void ABaseCharacter::DodgeEnd()
{
	/* This function is overriden in the SlashCharacter*/
}

void ABaseCharacter::FinishVault()
{
	/* Reset Character Movement */
	GetCharacterMovement()->SetMovementMode(MOVE_Walking);
}

void ABaseCharacter::PlayHitReactMontage(const FName& SectionName)
{
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if (AnimInstance && HitReactMontage)
	{
		AnimInstance->Montage_Play(HitReactMontage);
		AnimInstance->Montage_JumpToSection(SectionName, HitReactMontage);
	}
}

void ABaseCharacter::DirectionalHitReact(const FVector& ImpactPoint)
{
	// Get the character's forward direction (normalized vector)
	const FVector Forward = GetActorForwardVector();

	// Create a vector from the impact point but flatten it to the character's Z (ignore vertical difference)
	const FVector ImpactLowered(ImpactPoint.X, ImpactPoint.Y, GetActorLocation().Z);

	// Get a direction vector from the character to the impact point (normalized)
	const FVector ToHit = (ImpactLowered - GetActorLocation()).GetSafeNormal();

	// Calculate the cosine of the angle between Forward and ToHit using dot product
	// Since both vectors are normalized, the result is equal to cos(theta)
	const double CosTheta = FVector::DotProduct(Forward, ToHit);
	
	// Use arc-cosine to get the actual angle in radians
	double Theta = FMath::Acos(CosTheta);

	// Convert angle from radians to degrees
	Theta = FMath::RadiansToDegrees(Theta);

	// Determine if the angle is to the left or right using the cross product
	// If the Z component is negative, the impact is on the left, so we flip the angle
	const FVector CrossProduct = FVector::CrossProduct(Forward, ToHit);
	if (CrossProduct.Z < 0.f)
	{
		Theta *= -1.f;
	}

	// Default to "FromBack" if none of the conditions match
	FName Section("FromBack");

	// Based on the angle, choose the correct hit reaction direction
	if (Theta >= -45.f && Theta < 45.f)
	{
		Section = FName("FromFront");
	}
	else if (Theta >= -135.f && Theta < -45.f)
	{
		Section = FName("FromLeft");
	}
	else if (Theta >= 45.f && Theta < 135.f)
	{
		Section = FName("FromRight");
	}
	
	PlayHitReactMontage(Section);
}

void ABaseCharacter::PlayHitSound(const FVector& ImpactPoint)
{
	if (HitSound)
	{
		UGameplayStatics::PlaySoundAtLocation(this, HitSound, ImpactPoint);
	}
}

void ABaseCharacter::SpawnHitParticles(const FVector& ImpactPoint)
{
	if (HitParticle && GetWorld())
	{
		UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), HitParticle, ImpactPoint);
	}
}

void ABaseCharacter::HandleDamage(float DamageAmount)
{
	/* If the character has the attribute component, it takes damage now */
	if (Attributes)
	{
		Attributes->ReceiveDamage(DamageAmount);
	}
}

void ABaseCharacter::PlayMontageSection(UAnimMontage* Montage, const FName& SectionName)
{
	/* Plays a section in any montage it receives */
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if (AnimInstance && Montage)
	{
		AnimInstance->Montage_Play(Montage);
		AnimInstance->Montage_JumpToSection(SectionName, Montage);
	}
}

int32 ABaseCharacter::PlayRandomMontageSection(UAnimMontage* Montage, const TArray<FName>& SectionNames)
{
	/* Flexible function, which takes a montage and associated section names from an array to play a random section in the montage
		Works for death, attack and other montages */
	if (SectionNames.Num() <= 0) return -1;
	const int32 MaxSectionIndex = SectionNames.Num() - 1;
	const int32 Selection = FMath::RandRange(0, MaxSectionIndex);
	PlayMontageSection(Montage, SectionNames[Selection]);
	return Selection;
}

int32 ABaseCharacter::PlayAttackMontage()
{
	return PlayRandomMontageSection(ActiveAttackMontage, AttackMontageSections);
}

int32 ABaseCharacter::PlayDeathMontage()
{
	/* Select a random death from the death montage and play it. Also set the dead pose, which plays after the DeathMontage plays, which keeps the character permanently in a dead pose.
	 */
	const int32 Selection = PlayRandomMontageSection(DeathMontage, DeathMontageSections);

	TEnumAsByte<EDeathPose> Pose(Selection);
	if (Pose < EDeathPose::EDP_Max)
	{
		DeathPose = Pose;
	}
	return Selection;
}

void ABaseCharacter::PlayDodgeMontage()
{
	PlayMontageSection(DodgeMontage, FName("Default"));
}

void ABaseCharacter::PlayVaultMontage()
{
	/* The montage can be expanded with more vault animations, so small, medium and large obstacles could have unique vault animations*/
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if (AnimInstance && VaultMontage)
	{
		AnimInstance->Montage_Play(VaultMontage);
	}
}

void ABaseCharacter::StopAttackMontage()
{
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if (AnimInstance)
	{
		AnimInstance->Montage_Stop(0.25f, ActiveAttackMontage);
	}
}

FVector ABaseCharacter::GetTranslationWarpTarget()
{
	/* If there is a combat target, draw a vector from the location of this actor to the combat target and normalize it
	  The resulting vector is multiplied with the distance we want the character to maintain from the combat target when we end the motion warp to prevent overlapping*/
	if (CombatTarget == nullptr) return FVector();

	const FVector CombatTargetLocation = CombatTarget->GetActorLocation();
	const FVector Location = GetActorLocation();

	FVector TargetToMe = (Location - CombatTargetLocation).GetSafeNormal();
	TargetToMe *= WarpTargetDistance;

	return CombatTargetLocation + TargetToMe;
	
}

FVector ABaseCharacter::GetRotationWarpTarget()
{
	if (CombatTarget)
	{
			return CombatTarget->GetActorLocation();
	}
	return FVector();
}

void ABaseCharacter::DisableCapsule()
{
	GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}

bool ABaseCharacter::CanAttack()
{
	/* This will be overridden in children deriving from BaseCharacter */
	return false;
}

bool ABaseCharacter::IsAlive()
{
	return Attributes && Attributes->IsAlive();
}

void ABaseCharacter::DisableMeshCollision()
{
	GetMesh()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}

void ABaseCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	SetCombatMotionWarp();
}

void ABaseCharacter::SetWeaponProperties(EWeaponType WeaponType, float StaminaCost)
{
	/* After picking up a new weapon, update the weapon properties such as stamina consumption per attack and the animation montage corresponding to the weapon type
		This allows the player to smoothly transition between various movesets and can easily be expanded with new weapons as long as animations exist*/
	
	IWeaponInterface::SetWeaponProperties(WeaponType, StaminaCost);
	StaminaAttackCost = StaminaCost;
	switch (WeaponType)
	{
	case EWeaponType::EWT_AxeOneHanded:
		ActiveAttackMontage = AxeOneHandedAttackMontage;
		AttackMontageSections = AttackMontageSectionsAxeOneHanded;
		break;

	case EWeaponType::EWT_SwordOneHanded:
		ActiveAttackMontage = SwordOneHandedAttackMontage;
		AttackMontageSections = AttackMontageSectionsSwordOneHanded;
		break;
		
	default:
		ActiveAttackMontage = SwordOneHandedAttackMontage;
		AttackMontageSections = AttackMontageSectionsSwordOneHanded;
	}
}

void ABaseCharacter::SetVaultWarpLocations(const FVector Origin, const FVector Start, const FVector Middle, const FVector End)
{
	/* Receive the motion warp locations from the vault obstacle for the animation montage
	 */
	IVaultInterface::SetVaultWarpLocations(Origin, Start, Middle, End);
	VaultOrigin = Origin;
	VaultStartPosition = Start;
	VaultMiddlePosition = Middle;
	VaultEndPosition = End;
}

void ABaseCharacter::CanVault(const bool bVault)
{
	/* Update characters ability to vault, set via the VaultInterface from a VaultObstacle
	 */
	IVaultInterface::CanVault(bVault);
	bCanVault = bVault;
}

void ABaseCharacter::SetWeaponCollisionEnabled(ECollisionEnabled::Type CollisionEnabled)
{
	/* When character has a weapon and the weapon has a box collision, enable its collision to overlap with damageable actors and reset its IgnoreActors-array
	 */
	if (EquippedWeapon && EquippedWeapon->GetWeaponBox())
	{
		EquippedWeapon->GetWeaponBox()->SetCollisionEnabled(CollisionEnabled);
		EquippedWeapon->IgnoreActors.Empty();
	}
}

void ABaseCharacter::SetCombatMotionWarp()
{
	/* If there is a valid combat Target receive the position and rotation for the motion warp in the attack montage
	   If there is no valid target, remove the warp targets so the root motion animation plays normally*/
	if (CombatTarget)
	{
		FVector TranslationWarp = GetTranslationWarpTarget();
		FVector RotationWarp = GetRotationWarpTarget();

		MotionWarpingComponent->AddOrUpdateWarpTargetFromLocation("TranslationTarget", TranslationWarp);
		MotionWarpingComponent->AddOrUpdateWarpTargetFromLocation("RotationTarget", RotationWarp);
	}
	else
	{
		MotionWarpingComponent->RemoveWarpTarget("TranslationTarget");
		MotionWarpingComponent->RemoveWarpTarget("RotationTarget");
	}
	
}