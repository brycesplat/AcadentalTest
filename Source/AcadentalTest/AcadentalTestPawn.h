// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "PhysicsEngine/PhysicsHandleComponent.h"
#include "Camera/CameraComponent.h"
#include "Misc/DateTime.h"
#include "Misc/FileHelper.h"
#include "AcadentalTestPawn.generated.h"

UCLASS()
class ACADENTALTEST_API AAcadentalTestPawn : public APawn
{
	GENERATED_BODY()

public:
	// Sets default values for this pawn's properties
	AAcadentalTestPawn();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	UPhysicsHandleComponent* PhysicsHandle;
	UPrimitiveComponent* HitComponent;

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere)
	AActor* HitActor;

	APlayerController* controller;
	FString Filename;

	UPROPERTY(BlueprintReadOnly)
	bool SimulatingPhysics;
	UPROPERTY(BlueprintReadOnly)
	FDateTime CurrentSession;
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	float MaxDistance;
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	float DistanceToObject;
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	FVector RelativeGrabLocation;
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	float ScaleDelta;
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	float RotationModifier;
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	float CounterRotationModifier;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	bool ControllerCheck();

	UFUNCTION(Blueprintcallable)
		void GetManipulateActor();

	UFUNCTION(BlueprintCallable)
		void PhysicsTransform();

	UFUNCTION(BlueprintCallable)
		void NormalTransform();

		void CalculateRotationModifier(float& CameraYaw);

	UFUNCTION(BlueprintCallable)
		void RotateSetActor(float CameraYaw);

	UFUNCTION(BlueprintCallable)
		void Rotate();

	UFUNCTION(BlueprintCallable)
		void Scale(float scale);

	UFUNCTION(BlueprintCallable)
		FString LogActionToFile(FString Action, FString ObjectName, FString Data);
};
