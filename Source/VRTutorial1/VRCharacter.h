// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "MotionControllerComponent.h"
#include "VRCharacter.generated.h"

UCLASS()
class VRTUTORIAL1_API AVRCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AVRCharacter();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

private: //Configuration parameters
	UPROPERTY(EditAnywhere)
		float CameraFadeDuaration = 1.f;
	UPROPERTY(EditAnywhere)
		FVector TeleportProjectionExtent = FVector(50.0f, 50.0f, 50.0f);
	UPROPERTY(EditAnywhere)
		float blinkerRadius = 0.6f;
	UPROPERTY(EditAnywhere)
		float TeleportRayLength = 7000.f;
	UPROPERTY(EditAnywhere)
		float TeleportProjectileRadius = 10.f;
	UPROPERTY(EditAnywhere)
		float TeleportProjectileSpeed = 800.f;
	UPROPERTY(EditAnywhere)
		float TeleportSimulationTime = 1.f;

	FTimerHandle TeleportTimerHandle;

	void CorrectionBodyLocation();
	void UpdateDestinationMarker();
	bool FindTeleportDestination(TArray<FVector> &OutPath, FVector &OutLocation);
	void StartFade(float alphaStart, float alphaFin);
	void UpdateBlinkers();
	void UpdateSpline(TArray<FVector>& Path);
	void DrawTeleportPath(TArray<FVector>& Path);

	UPROPERTY()
	class UCameraComponent* Camera;
	UPROPERTY(VisibleAnywhere)
		UMotionControllerComponent* RightMotionController;
	UPROPERTY(VisibleAnywhere)
		class UVRHandStaticMeshComponent* RightMotionControllerMesh;

	UPROPERTY(VisibleAnywhere)
		UMotionControllerComponent* LeftMotionController;
	UPROPERTY(VisibleAnywhere)
		class UVRHandStaticMeshComponent* LeftMotionControllerMesh;

	UPROPERTY()
	class AHandController* LeftController;
	UPROPERTY()
	class AHandController* RightController;


	UPROPERTY()
	class USceneComponent* VRRoot;
	UPROPERTY(VisibleAnywhere)
		class USplineComponent* TeleportPath;
	UPROPERTY(VisibleAnywhere)
	class UStaticMeshComponent* DestinationMarker;
	UPROPERTY(VisibleAnywhere)
	class UPostProcessComponent* PostProcessComponent;
	UPROPERTY(EditAnywhere)
	class UMaterialInterface* BlinkerMaterialBase;
	UPROPERTY()
	class UMaterialInstanceDynamic* BlinkerInstanceMaterial;
	UPROPERTY(EditAnywhere)
	class UCurveFloat* CurveRadiusVelocity;
	UPROPERTY()
		TArray<class USplineMeshComponent*> TeleportPathMeshPool;

	UPROPERTY(EditDefaultsOnly)
		class UStaticMesh* TeleportArchMesh;
	UPROPERTY(EditDefaultsOnly)
		class UMaterialInterface* TeleportArchMaterial;

	UPROPERTY(EditDefaultsOnly)
		TSubclassOf<AHandController> HandControllerClass;

	void GripLeft();
	void ReleaseLeft();
	void GripRight();
	void ReleaseRight();

	//Input functions
	void MoveForward(float AxisValue);
	void MoveRight(float AxisValue);
	void BeginTeleport();
	void FinishTeleport();
	FVector2D GetBlinkerCentre();
};
