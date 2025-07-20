// Fill out your copyright notice in the Description page of Project Settings.


#include "Items/Weapons/Weapon.h"
#include "Characters/SlashCharacter.h"
#include "Kismet/GameplayStatics.h"
#include "Components/SphereComponent.h"
#include "Components/BoxComponent.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Interfaces/HitInterface.h"
#include "NiagaraComponent.h"
#include "Interfaces/WeaponInterface.h"

AWeapon::AWeapon()
{
	/* Set up Weapon */
	WeaponBox = CreateDefaultSubobject<UBoxComponent>(TEXT("WeaponBox"));
	WeaponBox->SetupAttachment(GetRootComponent());
	WeaponBox->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	WeaponBox->SetCollisionResponseToAllChannels(ECR_Overlap);
	WeaponBox->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Ignore);

	BoxTraceStart = CreateDefaultSubobject<USceneComponent>(TEXT("Box Trace Start"));
	BoxTraceStart->SetupAttachment(GetRootComponent());
	BoxTraceEnd = CreateDefaultSubobject<USceneComponent>(TEXT("Box Trace End"));
	BoxTraceEnd->SetupAttachment(GetRootComponent());
}

void AWeapon::BeginPlay()
{
	Super::BeginPlay();
	/* Bind to box collision begin overlap */
	WeaponBox->OnComponentBeginOverlap.AddDynamic(this, &AWeapon::OnBoxOverlap);
	BoxTraceExtent = FVector(WeaponBox->GetScaledBoxExtent().X, WeaponBox->GetScaledBoxExtent().Y, 5.f);
}

void AWeapon::AttachMeshToSocket(USceneComponent* InParent, FName InSocketName)
{
	/* Attach this mesh to a socket in the actor */
	FAttachmentTransformRules TransformRules(EAttachmentRule::SnapToTarget, true);
	ItemMesh->AttachToComponent(InParent, TransformRules, InSocketName);
}

void AWeapon::PlayEquipSound()
{
	if (EquipSound)
	{
		UGameplayStatics::PlaySoundAtLocation(this, EquipSound, GetActorLocation());
	}
}

void AWeapon::DisableSphereCollision()
{
	/* Once equipped, this weapon doesn't need sphere collision anymore */
	if (SphereComponent)
	{
		SphereComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	}
}

void AWeapon::DeactivateEmbers()
{
	if (ItemEffect)
	{
		ItemEffect->Deactivate();
	}
}

void AWeapon::Equip(USceneComponent* InParent, FName InSocketName, AActor* NewOwner, APawn* NewInstigator)
{
	/* Initialize the weapon for when its being picked up */
	ItemState = EItemState::EIS_Equipped;
	SetOwner(NewOwner);
	SetInstigator(NewInstigator);
	AttachMeshToSocket(InParent, InSocketName);
	DisableSphereCollision();
	
	if (NewOwner->ActorHasTag("Player")) PlayEquipSound(); // Only play sounds when player equips the weapon
	DeactivateEmbers();

	/* Update this weapon's properties in the owning actor for their attribute component */
	IWeaponInterface* WeaponInterface = Cast<IWeaponInterface>(GetOwner());
	if (WeaponInterface)
	{
		WeaponInterface->SetWeaponProperties(WeaponType, StaminaCost);
	}

}

void AWeapon::ExecuteGetHit(FHitResult BoxHit)
{
	IHitInterface* HitInterface = Cast<IHitInterface>(BoxHit.GetActor());
	if (HitInterface)
	{
		HitInterface->Execute_GetHit(BoxHit.GetActor(), BoxHit.ImpactPoint, GetOwner());
	}
}

bool AWeapon::ActorIsSameType(AActor* OtherActor)
{
	/* Prevent enemies and player from hitting themselves as well as enemies hitting other enemies with their own attacks*/
	if (GetOwner()->ActorHasTag(TEXT("Enemy")) && OtherActor->ActorHasTag(TEXT("Enemy")))
	{
		return true;
	}
	else if (GetOwner()->ActorHasTag(TEXT("Player")) && OtherActor->ActorHasTag(TEXT("Player")))
	{
		return true;
	}
	else
	{
		return false;
	}
}

void AWeapon::OnBoxOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp,
                           int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	/* Perform a single box trace when overlapping with an actor during the attack animation */
	if (ActorIsSameType(OtherActor) || OtherActor->ActorHasTag("Dead")) return;

	FHitResult BoxHit;
	BoxTrace(BoxHit);
	
	if (BoxHit.GetActor())
	{
		if (ActorIsSameType(BoxHit.GetActor()))	return; // Ignore if actor is the same type as themselves (for example player hitting player)

		/* Apply Damage to the target */
		UGameplayStatics::ApplyDamage(BoxHit.GetActor(), Damage, GetInstigator()->GetController(), this, UDamageType::StaticClass());
		ExecuteGetHit(BoxHit);
		CreateFields(BoxHit.ImpactPoint);
	}
}

void AWeapon::BoxTrace(FHitResult& BoxHit)
{
	/*Flip Start and End around to change direction of the trace
	const FVector Start = BoxTraceStart->GetComponentLocation();
	const FVector End = BoxTraceEnd->GetComponentLocation();*/

	/* Perform the box trace, perfectly aligning with the box collision
	 * Make sure when the box collision overlaps, the box trace also finds the desired actor, otherwise hits would simply not hit
	 */
	const FVector Center = WeaponBox->GetComponentLocation();
	const FRotator Rotation = WeaponBox->GetComponentRotation();
	const FVector Offset = FVector(0.f, 0.f, WeaponBox->GetScaledBoxExtent().Z - BoxTraceExtent.Z);
	const FVector Start = Center + Rotation.RotateVector(-Offset);
	const FVector End = Center + Rotation.RotateVector(Offset);
	
	TArray<AActor*> ActorsToIgnore;
	ActorsToIgnore.Add(this);
	ActorsToIgnore.Add(GetOwner());

	for (AActor* Actor : IgnoreActors)
	{
		ActorsToIgnore.Add(Actor);
	}

	UKismetSystemLibrary::BoxTraceSingle(this, Start, End, BoxTraceExtent,
		BoxTraceStart->GetComponentRotation(), ETraceTypeQuery::TraceTypeQuery1,
		false, ActorsToIgnore, bShowBoxDebug ? EDrawDebugTrace::ForDuration : EDrawDebugTrace::None, BoxHit, true);

	IgnoreActors.AddUnique(BoxHit.GetActor());
}
