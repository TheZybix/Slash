// Fill out your copyright notice in the Description page of Project Settings.


#include "Characters/SlashCharacter.h"
#include "Components/InputComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Components/AttributeComponent.h"
#include "Components/SphereComponent.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GroomComponent.h"
#include "Items/Item.h"
#include "Items/Weapons/Weapon.h"
#include "Animation/AnimMontage.h"
#include "HUD/DeathScreen.h"
#include "HUD/VictoryScreen.h"
#include "HUD/SlashHUD.h"
#include "HUD/SlashOverlay.h"
#include "Items/HealthPotion.h"
#include "Items/Soul.h"
#include "Items/Treasure.h"
#include "Kismet/GameplayStatics.h"


ASlashCharacter::ASlashCharacter()
{
 	
	PrimaryActorTick.bCanEverTick = true;
	
	//Add springarm to player character and attach it to root component
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>("CameraBoom");
	CameraBoom->SetupAttachment(GetRootComponent());
	CameraBoom->bUsePawnControlRotation = true;
	CameraBoom->TargetArmLength = 300.0f;
	
	//Add camera to player character and attach it to springarm
	Camera = CreateDefaultSubobject<UCameraComponent>("Camera");
	Camera->SetupAttachment(CameraBoom);
	Camera->bUsePawnControlRotation = false;

	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	//Rotate character according to controller forward + right vector
	GetCharacterMovement()->bOrientRotationToMovement = true;
	GetCharacterMovement()->RotationRate = FRotator(0.f, 400.f, 0.f);

	GetMesh()->SetCollisionObjectType(ECC_WorldDynamic);
	GetMesh()->SetCollisionResponseToAllChannels(ECR_Ignore);
	GetMesh()->SetCollisionResponseToChannel(ECC_Visibility, ECR_Block);
	GetMesh()->SetCollisionResponseToChannel(ECC_WorldDynamic, ECR_Overlap);
	GetMesh()->SetGenerateOverlapEvents(true);
	
	//Add hair to player and attach it to head socket, repeat for eyebrows
	Hair = CreateDefaultSubobject<UGroomComponent>(TEXT("Hair"));
	Hair->SetupAttachment(GetMesh());
	Hair->AttachmentName = FString("head");

	Eyebrows = CreateDefaultSubobject<UGroomComponent>(TEXT("Eyebrows"));
	Eyebrows->SetupAttachment(GetMesh());
	Eyebrows->AttachmentName = FString("head");

	Sphere = CreateDefaultSubobject<USphereComponent>(TEXT("TargetingSphere"));
	Sphere->SetupAttachment(GetRootComponent());
}

void ASlashCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	
	if (Attributes && SlashOverlay && !IsDodging && ActionState != EActionState::EAS_Attacking) //Only regen stamina when not attacking or dodging
	{
		Attributes->RegenStamina(DeltaTime);
		SlashOverlay->SetStaminaBarPercentage(Attributes->GetStaminaPercent());
	}
}

void ASlashCharacter::BeginPlay()
{
	Super::BeginPlay();

	Tags.Add(FName("Player")); // Tag this character as player for vault obstacles and weapon functionality

	/* Use the enhanced input system to bind button presses to functions, set up the mapping context and input actions in the editor */
	APlayerController* PlayerController = Cast<APlayerController>(GetController());
	if (PlayerController)
	{
		if(UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
		{
			Subsystem->AddMappingContext(SlashContext, 0); 		
		}
		
		/* Initialize the overlay showing health, stamina, gold and souls */
		InitializeSlashOverlay(PlayerController);
	}
}

void ASlashCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	/* Set up all Input Actions */
	if(UEnhancedInputComponent* EnhancedInputComponent = CastChecked<UEnhancedInputComponent>(PlayerInputComponent))
	{
		EnhancedInputComponent->BindAction(MovementAction, ETriggerEvent::Triggered, this, &ASlashCharacter::Move);
		EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &ASlashCharacter::Look);
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Started, this, &ASlashCharacter::Jump);
		EnhancedInputComponent->BindAction(EKeyAction, ETriggerEvent::Started, this, &ASlashCharacter::EKeyPressed);
		EnhancedInputComponent->BindAction(AttackAction, ETriggerEvent::Triggered, this, &ASlashCharacter::Attack);
		EnhancedInputComponent->BindAction(DodgeAction, ETriggerEvent::Triggered, this, &ASlashCharacter::Dodge);
		EnhancedInputComponent->BindAction(InteractAction, ETriggerEvent::Triggered, this, &ASlashCharacter::Interact);
	}
}

float ASlashCharacter::TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent,
                                  class AController* EventInstigator, AActor* DamageCauser)
{
	/* Take damage and update HUD */
	HandleDamage(DamageAmount);
	SetHUDHealth();
	return DamageAmount;
}

void ASlashCharacter::Move(const FInputActionValue& Value)
{
	if (ActionState == EActionState::EAS_IdleSitting) StandUp(); // On game begin, the character sits and stands up when player attempts to move
	if (ActionState != EActionState::EAS_Unoccupied) return; // Can only move if the ActionState is unoccupied

	const FVector2D MovementVector = Value.Get<FVector2D>();

	//Get rotation from controller instead of player root
	const FRotator Rotation = Controller->GetControlRotation();
	const FRotator YawRotation(0.f, Rotation.Yaw, 0.f);
	
	//Determine forward direction from Rotation Matrix X, add it to movement input
	const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
	AddMovementInput(ForwardDirection, MovementVector.Y);
	
	//Determine right direction from Rotation Matrix Y, add it to movement input
	const FVector RightDirection = FRotationMatrix(Rotation).GetUnitAxis(EAxis::Y);
	AddMovementInput(RightDirection, MovementVector.X);

	DetermineDodgeRotation(ForwardDirection, RightDirection, MovementVector);
}

void ASlashCharacter::Look(const FInputActionValue& Value)
{
	const FVector2D LookAxisValue = Value.Get<FVector2D>();
	if(GetController())
	{
		AddControllerYawInput(LookAxisValue.X);
		AddControllerPitchInput(LookAxisValue.Y);
	}
}

void ASlashCharacter::Jump()
{
	//Super::Jump();

	//Vault instead of jump
	Vault();
}

void ASlashCharacter::GetHit_Implementation(const FVector& ImpactPoint, AActor* Hitter)
{
	Super::GetHit_Implementation(ImpactPoint, Hitter);

	if (Attributes && Attributes->GetHealthPercent()>0.f) ActionState = EActionState::EAS_HitReaction; //Hitreact if health is above 0 after taking damage

	SetWeaponCollisionEnabled(ECollisionEnabled::NoCollision); //Reset weapon collision after taking damage and not finishing the attack montage
}

void ASlashCharacter::SetOverlappingItem(class AItem* Item)
{
	OverlappingItem = Item;
}

void ASlashCharacter::AddSouls(ASoul* Soul)
{
	if (Attributes && SlashOverlay)
	{
		Attributes->AddSouls(Soul->GetSouls());
		SlashOverlay->SetSouls(Attributes->GetSouls());
	}
}

void ASlashCharacter::AddGold(ATreasure* Treasure)
{
	if (Attributes && SlashOverlay)
	{
		Attributes->AddGold(Treasure->GetGold());
		SlashOverlay->SetGold(Attributes->GetGold());
	}
}

void ASlashCharacter::AddHealth(AHealthPotion* Health)
{
	if (Attributes && SlashOverlay)
	{
		Attributes->ReceiveHealing(Health->GetHealth());
		SlashOverlay->SetHealthBarPercentage(Attributes->GetHealthPercent());
	}
}

void ASlashCharacter::EKeyPressed()
{
	AWeapon* OverlappingWeapon = Cast<AWeapon>(OverlappingItem);
	
	/* If there's an overlapping weapon, equip it if player currently has no weapon
	 Otherwise destroy the current one and equip the new one
	 If there is no overlapping weapon and the equipped weapon is valid, arm or disarm it depending on CharacterState */
	if (OverlappingWeapon)
	{
		if (EquippedWeapon)
		{
			EquippedWeapon->Destroy();
		}
		EquipWeapon(OverlappingWeapon);
	}
	else if (EquippedWeapon !=nullptr)
	{
		if (CanDisarm())
		{
			Disarm();
		}
		else if (CanArm())
		{
			Arm();
		}
	}
}

void ASlashCharacter::Attack()
{
	Super::Attack();
	
	/* When player can attack, they attack and use stamina as well as updating their state to prevent other actions from being performed */
	if (CanAttack())
	{
		Attributes->UseStamina(Attributes->GetAttackCost());
		SlashOverlay->SetStaminaBarPercentage(Attributes->GetStaminaPercent());
		PlayAttackMontage();
		ActionState = EActionState::EAS_Attacking;
		FindCombatTarget();
	}
}

void ASlashCharacter::Dodge()
{
	if (ActionState != EActionState::EAS_Unoccupied || Attributes->GetStaminaPercent() < Attributes->GetDodgeCost()/100.f) return; // Dodge if the player is not performing other actions and has enough stamina

	/* When player can dodge, they dodge and use stamina as well as updating their state to prevent other actions from being performed */
	if (Attributes &&  SlashOverlay)
	{
		SetActorRotation(DodgeRotation);
		PlayDodgeMontage();
		ActionState = EActionState::EAS_Dodge;
		Attributes->UseStamina(Attributes->GetDodgeCost());
		SlashOverlay->SetStaminaBarPercentage(Attributes->GetStaminaPercent());
		IsDodging = true;
	}
}

void ASlashCharacter::Interact()
{
	if (bCanWin) Win();	//Currently interact will only check if player can win by overlapping with a tooltip actor that possess a win condition
}

void ASlashCharacter::Win()
{
	VictoryScreenUI();
	bCanWin = false;
}

void ASlashCharacter::EquipWeapon(AWeapon* Weapon)
{
	Weapon->Equip(GetMesh(), FName("RightHandSocket"), this, this);
	CharacterState = ECharacterState::ECS_EquippedOneHandedWeapon;
	OverlappingItem = nullptr;
	EquippedWeapon = Weapon;
	Attributes->SetWeaponStaminaCost(StaminaAttackCost);
}

void ASlashCharacter::AttackEnd()
{
	ActionState = EActionState::EAS_Unoccupied;
}

void ASlashCharacter::DodgeEnd()
{
	Super::DodgeEnd();

	ActionState = EActionState::EAS_Unoccupied;
	IsDodging = false;
}

bool ASlashCharacter::CanAttack()
{
	return ActionState == EActionState::EAS_Unoccupied &&
				CharacterState != ECharacterState::ECS_Unequipped &&
					EquippedWeapon &&
						Attributes->GetStaminaPercent() > Attributes->GetAttackCost()/100.f;
}

bool ASlashCharacter::CanDisarm()
{
	return ActionState == EActionState::EAS_Unoccupied &&
		CharacterState != ECharacterState::ECS_Unequipped;
}

bool ASlashCharacter::CanArm()
{
	return ActionState == EActionState::EAS_Unoccupied &&
		CharacterState == ECharacterState::ECS_Unequipped;
}

void ASlashCharacter::Disarm()
{
	PlayEquipMontage(FName("Unequip"));
	CharacterState = ECharacterState::ECS_Unequipped;
	ActionState = EActionState::EAS_Equipping;
}

void ASlashCharacter::Arm()
{
	PlayEquipMontage(FName("Equip"));
	CharacterState = ECharacterState::ECS_EquippedOneHandedWeapon;
	ActionState = EActionState::EAS_Equipping;
}

void ASlashCharacter::AttachWeaponToBack()
{
	if (EquippedWeapon)
	{
		EquippedWeapon->AttachMeshToSocket(GetMesh(), FName("spine_socket"));
	}
}

void ASlashCharacter::AttachWeaponToHand()
{
	if (EquippedWeapon)
	{
		EquippedWeapon->AttachMeshToSocket(GetMesh(), FName("RightHandSocket"));
	}
}

void ASlashCharacter::PlayEquipMontage(const FName& SectionName)
{
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if (AnimInstance && EquipMontage)
	{
		AnimInstance->Montage_Play(EquipMontage);
		AnimInstance->Montage_JumpToSection(SectionName, EquipMontage);
	}
}

void ASlashCharacter::PlayStandUpMontage()
{
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if (AnimInstance && StandingUpMontage)
	{
		AnimInstance->Montage_Play(StandingUpMontage);
	}
}

void ASlashCharacter::DeathScreenUI()
{
	/* Add deathscreen widget to viewport and set input mode to UI only */
	if (DeathScreenClass)
	{
		DeathScreen = CreateWidget<UDeathScreen>(GetWorld(), DeathScreenClass);
		APlayerController* PlayerController = UGameplayStatics::GetPlayerController(GetWorld(), 0);

		if (DeathScreen && PlayerController)
		{
			DeathScreen->AddToViewport();
			PlayerController->bShowMouseCursor = true;
			
			FInputModeUIOnly InputMode;
			InputMode.SetWidgetToFocus(DeathScreen->TakeWidget());
			PlayerController->SetInputMode(InputMode);
		}
	}
}

void ASlashCharacter::VictoryScreenUI()
{
	/* Add victoryscreen widget to viewport and set input mode to UI only */
	if (VictoryScreenClass)
	{
		VictoryScreen = CreateWidget<UVictoryScreen>(GetWorld(), VictoryScreenClass);
		APlayerController* PlayerController = UGameplayStatics::GetPlayerController(GetWorld(), 0);

		if (VictoryScreen && PlayerController)
		{
			VictoryScreen->AddToViewport();
			PlayerController->bShowMouseCursor = true;
			
			FInputModeUIOnly InputMode;
			InputMode.SetWidgetToFocus(VictoryScreen->TakeWidget());
			PlayerController->SetInputMode(InputMode);
		}
	}
}

void ASlashCharacter::Die_Implementation()
{
	/* Hide the overlay on death and set action state to dead */
	Super::Die_Implementation();
	ActionState = EActionState::EAS_Dead;
	SlashOverlay->SetVisibility(ESlateVisibility::Hidden);
	DisableMeshCollision();

	DeathScreenUI();
}

void ASlashCharacter::SetCombatTarget(AActor* Target)
{
	CombatTarget = Target;
}

void ASlashCharacter::ClearCombatTarget()
{
	CombatTarget = nullptr;
}

void ASlashCharacter::GetTargetsInRange(TArray<AActor*>& Targets)
{
	/* Get all targets in range of the sphere collision */
	Sphere->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	GetOverlappingActors(Targets, TargetClassFilter);
	Sphere->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}

void ASlashCharacter::FindCombatTarget()
{
	TArray<AActor*> Targets;
	GetTargetsInRange(Targets);
	ClearCombatTarget();
	
	if (Targets.IsEmpty()) return; // Do nothing if the sphere doesn't overlap any actors

	AActor* NewTarget = ClosestCombatTarget(Targets); //Select a new combat target from the Targets array
	if (NewTarget) SetCombatTarget(NewTarget);
}

AActor* ASlashCharacter::ClosestCombatTarget(TArray<AActor*>& Targets)
{
	FVector CameraPosition = Camera->GetComponentLocation();
	FVector CameraAim = Camera->GetForwardVector();

	float MaxDotProduct = -1.f;
	int MaxIndex = -1;
	AActor* Actor;

	for (int i = 0; i < Targets.Num(); ++i)
	{
		Actor = Targets[i];
		FVector ToTarget = (Actor->GetActorLocation() - CameraPosition).GetSafeNormal();
            
			/* The dot product will be highest for the actor closest to the center of the screen */
			float DotProduct = FVector::DotProduct(CameraAim, ToTarget);
			if (DotProduct > MaxDotProduct)
			{
				/* Keep the highest result to compare with subsequent results */
				MaxDotProduct = DotProduct;
				
				/* Only select the actor with the highest dot product */
				MaxIndex = i;
		}
	}
	if (MaxIndex >= 0) return Targets[MaxIndex];
	return nullptr;
}

void ASlashCharacter::Vault()
{
	if (ActionState == EActionState::EAS_Unoccupied && bCanVault)
	{
		Super::Vault();
		ActionState = EActionState::EAS_Vault;
	}
}

void ASlashCharacter::FinishVault()
{
	Super::FinishVault();
	ActionState = EActionState::EAS_Unoccupied;
}

void ASlashCharacter::FinishEquipping()
{
	ActionState = EActionState::EAS_Unoccupied;
}

void ASlashCharacter::HitReactEnd()
{
	ActionState = EActionState::EAS_Unoccupied;
}

void ASlashCharacter::DodgeRollInvincibility()
{
	GetMesh()->SetCollisionResponseToChannel(ECollisionChannel::ECC_WorldDynamic, ECR_Ignore); // Give the player I-frames, so even overlapping enemy weapons won't hit during the dodge
}

void ASlashCharacter::DodgeRollInvincibilityEnd()
{
	GetMesh()->SetCollisionResponseToChannel(ECollisionChannel::ECC_WorldDynamic, ECR_Overlap);
}

void ASlashCharacter::StandUpEnd()
{
	ActionState = EActionState::EAS_Unoccupied;
}

void ASlashCharacter::InitializeSlashOverlay(APlayerController* PlayerController)
{
	ASlashHUD* SlashHUD = Cast<ASlashHUD>(PlayerController->GetHUD());
	if (SlashHUD)
	{
		SlashOverlay = SlashHUD->GetSlashOverlay();
		if (SlashOverlay && Attributes)
		{
			SlashOverlay->SetHealthBarPercentage(Attributes->GetHealthPercent());
			SlashOverlay->SetStaminaBarPercentage(1.f);
			SlashOverlay->SetGold(0);
			SlashOverlay->SetSouls(0);
		}
	}
}

void ASlashCharacter::SetHUDHealth()
{
	if (SlashOverlay && Attributes) SlashOverlay->SetHealthBarPercentage(Attributes->GetHealthPercent());
}

void ASlashCharacter::StandUp()
{
	if (bReceiveInput)
	{
		bReceiveInput = false;
		PlayStandUpMontage();
	}
}

void ASlashCharacter::DetermineDodgeRotation(FVector ForwardDir, FVector RightDir, FVector2D InputValues)
{
	/* Make dodging more snappy by basing it on player input, not the current rotation of the character */
	FVector Velocity = GetVelocity();
	FVector DodgeDirection = (ForwardDir * InputValues.Y) + (RightDir * InputValues.X);
	DodgeDirection = DodgeDirection.GetSafeNormal();
	
	if (Velocity.IsNearlyZero())
	{
		DodgeDirection = GetActorForwardVector().GetSafeNormal();
	}
	DodgeRotation = DodgeDirection.Rotation();
}