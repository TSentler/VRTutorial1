// Fill out your copyright notice in the Description page of Project Settings.


#include "VRCharacter.h"
#include "Camera/CameraComponent.h"
#include "Components/InputComponent.h"
#include "Components/StaticMeshComponent.h"
#include "DrawDebugHelpers.h"

// Sets default values
AVRCharacter::AVRCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	VRRoot = CreateDefaultSubobject<USceneComponent>(TEXT("VRRoot"));
	VRRoot->SetupAttachment(GetRootComponent());

	Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	Camera->SetupAttachment(VRRoot);

	DestinationMarker = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("DestinationMarker"));
	DestinationMarker->SetupAttachment(GetRootComponent());

	//Take control of the default Player
	AutoPossessPlayer = EAutoReceiveInput::Player0;
}

// Called when the game starts or when spawned
void AVRCharacter::BeginPlay()
{
	Super::BeginPlay();
	
	DestinationMarker->SetVisibility(false);
}

// Called every frame
void AVRCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	CorrectionBodyLocation();

	UpdateDestinationMarker();

}

void AVRCharacter::UpdateDestinationMarker()
{
	FHitResult OutHit;
	FVector Start = Camera->GetComponentLocation();
	FVector End = (Start + 1000.f * Camera->GetForwardVector());
	FCollisionQueryParams CollisionParams;

	DrawDebugLine(GetWorld(), Start, End, FColor::Green, false, 1, 0, 1);

	if (GetWorld()->LineTraceSingleByChannel(OutHit, Start, End, ECC_Visibility))
	{
		DestinationMarker->SetVisibility(true);
		DestinationMarker->SetWorldLocation(OutHit.Location);
	}
	else
		DestinationMarker->SetVisibility(false);
}

// Called to bind functionality to input
void AVRCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	//Hook up every-frame handling for our four axes
	PlayerInputComponent->BindAxis("MoveForward", this, &AVRCharacter::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &AVRCharacter::MoveRight);
}

void AVRCharacter::CorrectionBodyLocation()
{
	FVector cameraOffset = Camera->GetComponentLocation() - GetActorLocation();
	cameraOffset.Z = .0f;
	if (cameraOffset.Size() > 20.f)
	{
		AddActorWorldOffset(cameraOffset);
		VRRoot->AddWorldOffset(-cameraOffset);
	}
}

//Input functions
void AVRCharacter::MoveForward(float AxisValue)
{
	AddMovementInput(AxisValue * Camera->GetForwardVector());
}

void AVRCharacter::MoveRight(float AxisValue)
{
	AddMovementInput(AxisValue * Camera->GetRightVector());
}

