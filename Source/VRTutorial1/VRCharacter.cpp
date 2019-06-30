// Fill out your copyright notice in the Description page of Project Settings.

#include "VRCharacter.h"
#include "Camera/CameraComponent.h"
#include "Components/InputComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Components/CapsuleComponent.h"
#include "DrawDebugHelpers.h"
#include "Kismet/GameplayStatics.h"
#include "TimerManager.h"
#include "NavigationSystem/Public/NavigationSystem.h"
#include "Components/PostProcessComponent.h"
#include "Classes/Materials/MaterialInstanceDynamic.h"
#include "Classes/GameFramework/CharacterMovementComponent.h"
#include "Classes/GameFramework/PlayerController.h"
#include "XRMotionControllerBase.h"
#include "Kismet/GameplayStatics.h"
#include "Components/SplineComponent.h"
#include "Components/SplineMeshComponent.h"
#include "MotionControllerComponent.h"
#include "HandController.h"
#include "VRHandStaticMeshComponent.h"



// Sets default values
AVRCharacter::AVRCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	VRRoot = CreateDefaultSubobject<USceneComponent>(TEXT("VRRoot"));
	VRRoot->SetupAttachment(GetRootComponent());

	Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	Camera->SetupAttachment(VRRoot);

	LeftMotionController = CreateDefaultSubobject<UMotionControllerComponent>(TEXT("LeftMotionController"));
	LeftMotionController->SetupAttachment(VRRoot);
	LeftMotionControllerMesh = CreateDefaultSubobject<UVRHandStaticMeshComponent>(TEXT("LeftMotionControllerMesh"));
	LeftMotionControllerMesh->SetupAttachment(LeftMotionController);
	LeftMotionController->SetTrackingSource(EControllerHand::Left);
	LeftMotionController->bDisplayDeviceModel = true;

	RightMotionController = CreateDefaultSubobject<UMotionControllerComponent>(TEXT("RightMotionController"));
	RightMotionController->SetupAttachment(VRRoot);
	RightMotionControllerMesh = CreateDefaultSubobject<UVRHandStaticMeshComponent>(TEXT("RightMotionControllerMesh"));
	RightMotionControllerMesh->SetupAttachment(RightMotionController);
	RightMotionController->SetTrackingSource(EControllerHand::Right);
	RightMotionController->bDisplayDeviceModel = true;

	TeleportPath = CreateDefaultSubobject<USplineComponent>(TEXT("RightTeleportPath"));
	TeleportPath->SetupAttachment(VRRoot);

	DestinationMarker = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("DestinationMarker"));
	DestinationMarker->SetupAttachment(GetRootComponent());
	
	PostProcessComponent = CreateDefaultSubobject<UPostProcessComponent>(TEXT("PostProcessComponent"));
	PostProcessComponent->SetupAttachment(GetRootComponent());
	
	
	// = UMaterialInstanceDynamic::Create(, this);
	//PostProcessComponent->Settings.Blendables_DEPRECATED[0] = DynamicMaterial;

	//PostProcessComponent->Settings.AddBlendable(BlinkerInstanceMaterial, 1);

	//Take control of the default Player
	AutoPossessPlayer = EAutoReceiveInput::Player0;
}

// Called when the game starts or when spawned
void AVRCharacter::BeginPlay()
{
	Super::BeginPlay();
	
	DestinationMarker->SetVisibility(false);

	if (BlinkerMaterialBase != nullptr)
	{
		BlinkerInstanceMaterial = UMaterialInstanceDynamic::Create(BlinkerMaterialBase, nullptr, FName(TEXT("BlinkerInstanceMaterial")));
		PostProcessComponent->AddOrUpdateBlendable(BlinkerInstanceMaterial);

	}

	//LeftController = GetWorld()->SpawnActor<AHandController>(HandControllerClass);
	//if (LeftController != nullptr)
	//{
	//	LeftController->AttachToActor(this, FAttachmentTransformRules::KeepRelativeTransform);
	//	LeftController->SetHand(EControllerHand::Left);
	//}
	//RightController = GetWorld()->SpawnActor<AHandController>(HandControllerClass);
	//if (RightController != nullptr)
	//{
	//	RightController->AttachToActor(this, FAttachmentTransformRules::KeepRelativeTransform);
	//	RightController->SetHand(EControllerHand::Right);
	//}
	//Source\Runtime\HeadMountedDisplay\Private\MotionControllerComponent.cpp
	//472
	//https://answers.unrealengine.com/questions/887343/421-422-motion-controllers-stopped-responding.html
	
	RightMotionControllerMesh->PairController(LeftMotionControllerMesh);
	LeftMotionControllerMesh->PairController(RightMotionControllerMesh);
}

// Called every frame
void AVRCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	UpdateBlinkers();

	CorrectionBodyLocation();

	UpdateDestinationMarker();


}

void AVRCharacter::UpdateBlinkers()
{
	if (BlinkerInstanceMaterial != nullptr && CurveRadiusVelocity != nullptr)
	{
		UCharacterMovementComponent* movementComp = GetCharacterMovement();
		float speed = GetVelocity().Size() / movementComp->MaxAcceleration;
		float radius = CurveRadiusVelocity->GetFloatValue(speed);
		BlinkerInstanceMaterial->SetScalarParameterValue(TEXT("Radius"), radius);

		FVector2D Centre = GetBlinkerCentre();
		BlinkerInstanceMaterial->SetVectorParameterValue(TEXT("Centre"), FLinearColor(Centre.X, Centre.Y, .0f));
	}
}

void AVRCharacter::DrawTeleportPath(TArray<FVector>& Path)
{
	UpdateSpline(Path);

	for (auto *SplineMesh : TeleportPathMeshPool)
	{
		SplineMesh->SetVisibility(false);
	}

	for (int32 i = 0; i < Path.Num(); ++i)
	{
		USplineMeshComponent* SplineMesh;

		if (TeleportPathMeshPool.Num() <= i)
		{
			SplineMesh = NewObject<USplineMeshComponent>(this);
			SplineMesh->SetMobility(EComponentMobility::Movable);
			SplineMesh->AttachToComponent(TeleportPath, FAttachmentTransformRules::KeepRelativeTransform);
			SplineMesh->SetStaticMesh(TeleportArchMesh);
			SplineMesh->SetMaterial(0, TeleportArchMaterial);
			SplineMesh->RegisterComponent();

			TeleportPathMeshPool.Add(SplineMesh);
		}
		
		SplineMesh = TeleportPathMeshPool[i];
		SplineMesh->SetVisibility(true);

		FVector StartPos, StartTangent, EndPos, EndTangent;
		TeleportPath->GetLocalLocationAndTangentAtSplinePoint(i, StartPos, StartTangent);
		TeleportPath->GetLocalLocationAndTangentAtSplinePoint(i + 1, EndPos, EndTangent);
		SplineMesh->SetStartAndEnd(StartPos, StartTangent, EndPos, EndTangent);
	}
}

void AVRCharacter::UpdateSpline(TArray<FVector>& Path)
{
	TeleportPath->ClearSplinePoints(false);
	for (int32 i = 0; i < Path.Num(); ++i)
	{
		FVector LocalPosition = TeleportPath->GetComponentTransform().InverseTransformPosition(Path[i]);
		FSplinePoint Point(i, LocalPosition, ESplinePointType::Curve);
		TeleportPath->AddPoint(Point, false);
	}

	TeleportPath->UpdateSpline();
}

FVector2D AVRCharacter::GetBlinkerCentre()
{
	FVector moveDir = GetVelocity().GetSafeNormal();
	if (moveDir.IsNearlyZero())
		return FVector2D(.5f, .5f);

	APlayerController* PC = Cast<APlayerController>(GetController());
	if (PC == nullptr)
		return FVector2D(.5f, .5f);

	int sizeX;
	int sizeY;
	PC->GetViewportSize(sizeX, sizeY);

	float backToForward = FMath::Sign(FVector::DotProduct(Camera->GetForwardVector(), moveDir));
	if (FMath::IsNearlyZero(backToForward))
		backToForward = 1.f;

	FVector2D ScreenStationaryLocation;
	FVector WorldStationaryLocation = Camera->GetComponentLocation() + backToForward * moveDir * 100.f;
	PC->ProjectWorldLocationToScreen(WorldStationaryLocation, ScreenStationaryLocation);
	return FVector2D(ScreenStationaryLocation.X / sizeX, ScreenStationaryLocation.X / sizeY);
}

void AVRCharacter::UpdateDestinationMarker()
{
	TArray<FVector> Path;
	FVector Location;
	if (FindTeleportDestination(Path, Location))
	{
		DestinationMarker->SetVisibility(true);
		DestinationMarker->SetWorldLocation(Location);
		DrawTeleportPath(Path);
	}
	else 
	{
		DestinationMarker->SetVisibility(false);

		TArray<FVector> EmptyPath;
		DrawTeleportPath(EmptyPath);
	}
}

bool AVRCharacter::FindTeleportDestination(TArray<FVector>& OutPath, FVector& OutLocation)
{
	FHitResult OutHit;
	FVector Start = RightMotionController->GetComponentLocation();
	FVector Look = RightMotionController->GetForwardVector();
	//Look = Look.RotateAngleAxis(30, RightController->GetRightVector());
	//FVector End = (Start + TeleportRayLength * Look);
	//FCollisionQueryParams CollisionParams;


	//DrawDebugLine(GetWorld(), Start, End, FColor::Green, false, 1, 0, 1);
	//bool bHit = GetWorld()->LineTraceSingleByChannel(OutHit, Start, End, ECC_Visibility);
	FPredictProjectilePathParams Params(
		TeleportProjectileRadius, 
		Start, 
		Look * TeleportProjectileSpeed,
		TeleportSimulationTime,
		ECollisionChannel::ECC_Visibility,
		this
	);
	Params.DrawDebugType = EDrawDebugTrace::ForOneFrame;
	Params.bTraceComplex = true;
	FPredictProjectilePathResult Result;
	bool bHit = UGameplayStatics::PredictProjectilePath(this, Params, Result);

	if (!bHit)
		return false;

	for (auto PointData : Result.PathData)
	{
		OutPath.Add(PointData.Location);
	}

	FNavLocation NavLoc;
	FNavAgentProperties NavAgentProps;
	bool bOnNavMesh = FNavigationSystem::GetCurrent<UNavigationSystemV1>(GetWorld())
		->ProjectPointToNavigation(Result.HitResult.Location, NavLoc, TeleportProjectionExtent);
	
	if (!bOnNavMesh)
		return false;

	OutLocation = NavLoc.Location;

	return true;
}

void AVRCharacter::StartFade(float alphaStart, float alphaFin)
{
	/*
		UGameplayStatics::GetPlayerController(GetWorld(), 0)->PlayerCameraManager
		->StartCameraFade(1.f, 0.f, CameraFadeDuaration, FLinearColor(0.f, 0.f, 0.f, 1.f));
		*/
	APlayerController* PC = Cast<APlayerController>(GetController());
	if (PC == nullptr)
		return;

	PC->PlayerCameraManager->StartCameraFade(alphaStart, alphaFin, CameraFadeDuaration, FLinearColor(0.f, 0.f, 0.f, 1.f));
}

// Called to bind functionality to input
void AVRCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	//Hook up every-frame handling for our four axes
	PlayerInputComponent->BindAxis("MoveForward", this, &AVRCharacter::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &AVRCharacter::MoveRight);
	PlayerInputComponent->BindAction("Teleport", IE_Released, this, &AVRCharacter::BeginTeleport);
	PlayerInputComponent->BindAction("GripLeft", IE_Pressed, this, &AVRCharacter::GripLeft);
	PlayerInputComponent->BindAction("GripLeft", IE_Released, this, &AVRCharacter::ReleaseLeft);
	PlayerInputComponent->BindAction("GripRight", IE_Pressed, this, &AVRCharacter::GripRight);
	PlayerInputComponent->BindAction("GripRight", IE_Released, this, &AVRCharacter::ReleaseRight);
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

void AVRCharacter::GripLeft()
{
	LeftMotionControllerMesh->Grip();
}
void AVRCharacter::ReleaseLeft()
{
	LeftMotionControllerMesh->ReleaseGrip();
}
void AVRCharacter::GripRight()
{
	RightMotionControllerMesh->Grip();
}
void AVRCharacter::ReleaseRight()
{
	RightMotionControllerMesh->ReleaseGrip();
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

void AVRCharacter::BeginTeleport()
{
	UWorld* World = GetWorld();
	if (World == nullptr)
		return;

	//World->GetTimerManager()
	GetWorldTimerManager().SetTimer(TeleportTimerHandle, this, &AVRCharacter::FinishTeleport, CameraFadeDuaration);
	
	StartFade(0.f, 1.f);
}

void AVRCharacter::FinishTeleport()
{
	FVector Destination = DestinationMarker->GetComponentLocation();
	Destination += FVector::UpVector * GetCapsuleComponent()->GetScaledCapsuleHalfHeight();
	SetActorLocation(Destination);
	StartFade(1.f, 0.f);
}

