// Fill out your copyright notice in the Description page of Project Settings.


#include "AcadentalTestPawn.h"
#include "Kismet/KismetMathLibrary.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Camera/CameraComponent.h"

// Sets default values
AAcadentalTestPawn::AAcadentalTestPawn()
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	SimulatingPhysics = false;
	MaxDistance = 2000;
	DistanceToObject = 0;
	RelativeGrabLocation = { 0,0,0 };
	ScaleDelta = 1;
	RotationModifier = 0;
	CounterRotationModifier = 0;
	CurrentSession = FDateTime();
	Filename = "LOGFILE_";
}

// Called when the game starts or when spawned
void AAcadentalTestPawn::BeginPlay()
{
	Super::BeginPlay();
	CurrentSession = FDateTime::Now();
	Filename.AppendInt(CurrentSession.GetYear());
	Filename.AppendChar('_');
	Filename.AppendInt(CurrentSession.GetMonth());
	Filename.AppendChar('_');
	Filename.AppendInt(CurrentSession.GetDay());
	Filename.AppendChar('_');
	Filename.AppendInt(CurrentSession.GetHour());
	Filename.AppendChar('_');
	Filename.AppendInt(CurrentSession.GetMinute());
	Filename.AppendChar('_');
	Filename.AppendInt(CurrentSession.GetSecond());
	Filename.Append(".txt");
}

// Called every frame
void AAcadentalTestPawn::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void AAcadentalTestPawn::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

// Checks if the Pawn is controlled by a PlayerController
bool AAcadentalTestPawn::ControllerCheck() {
	if (controller != NULL) {
		return true;
	}
	controller = AAcadentalTestPawn::GetController<APlayerController>();
	if (controller == NULL) {
		return false;
	}
	return true;
}

// Obtains the object that can be manipulated and determine if its a physics object or non-physics object
void AAcadentalTestPawn::GetManipulateActor() {
	if (!ControllerCheck()) {
		return;
	}

	FVector location, direction;
	FHitResult Hit;
	controller->DeprojectMousePositionToWorld(location, direction);
	direction.operator*=(FVector3d{ MaxDistance, MaxDistance, MaxDistance });

	FCollisionQueryParams TraceParams = FCollisionQueryParams(FName(TEXT("Trace")), true, this);
	TraceParams.bTraceComplex = true;
	TraceParams.bReturnPhysicalMaterial = false;
	FCollisionResponseParams ResponseParams ;

	if (
		!GetWorld()->LineTraceSingleByChannel(Hit, location, direction, ECC_Visibility, TraceParams, ResponseParams)//(Hit, location, direction, ECC_Visibility, FCollisionQueryParams(getself))
		) {
		return;
	}

	if (Hit.Component->Mobility != EComponentMobility::Movable) {
		return;
	}

	DistanceToObject = Hit.Distance;
	HitActor = Hit.GetActor();
	HitComponent = Hit.GetComponent();

	if (HitComponent->IsSimulatingPhysics()) {
		SimulatingPhysics = true;
		PhysicsHandle->GrabComponentAtLocation(HitComponent, NAME_None, Hit.Location);
	}
	else {
		RelativeGrabLocation = UKismetMathLibrary::InverseTransformLocation(UKismetMathLibrary::Conv_VectorToTransform(HitComponent->GetComponentLocation()), Hit.Location);
	}
}

// Transforms location of a physics object
void AAcadentalTestPawn::PhysicsTransform() {
	if (!ControllerCheck()) {
		return;
	}
	if (HitActor == NULL) {
		return;
	}

	FVector location, direction;
	controller->DeprojectMousePositionToWorld(location, direction);
	direction.operator*=(FVector3d{ DistanceToObject, DistanceToObject, DistanceToObject });
	PhysicsHandle->SetTargetLocation(location.operator+(direction));
}

// Transforms location of a non-physics object
void AAcadentalTestPawn::NormalTransform() {
	if (!ControllerCheck()) {
		return;
	}
	if (HitActor == NULL) {
		return;
	}

	FVector location, direction;
	controller->DeprojectMousePositionToWorld(location, direction);
	direction.operator*=(FVector3d{ DistanceToObject, DistanceToObject, DistanceToObject });
	location.operator+=(direction);

	HitActor->SetActorLocation(UKismetMathLibrary::InverseTransformLocation(UKismetMathLibrary::Conv_VectorToTransform(RelativeGrabLocation), location));
}

// Calculates the ratio and direction of Pitch and Roll rotation
void AAcadentalTestPawn::CalculateRotationModifier(float& CameraYaw) {
	if (HitActor == NULL) {
		return;
	}

	RotationModifier = UKismetMathLibrary::DegCos(CameraYaw);
	CounterRotationModifier = UKismetMathLibrary::DegSin(CameraYaw) * -1;
}

// Sets Actor as the target
void AAcadentalTestPawn::RotateSetActor(float CameraYaw) {
	if (!ControllerCheck()) {
		return;
	}
	if (HitActor == NULL) {
		FHitResult Hit;

		if (!controller->GetHitResultUnderCursorByChannel(TraceTypeQuery1, true, Hit)) {
			return;
		}

		if (Hit.Component->Mobility != EComponentMobility::Movable) {
			return;
		}
		HitComponent = Hit.GetComponent();
		HitActor = Hit.GetActor();
	}
	CalculateRotationModifier(CameraYaw);
}

// Rotate targeted Actor
void AAcadentalTestPawn::Rotate() {
	if (!ControllerCheck()) {
		return;
	}
	if (HitActor == NULL) {
		return;
	}

	double x, y;
	controller->GetInputMouseDelta(x, y);

	HitActor->AddActorWorldRotation(FQuat{ FRotator{ y * -1 * RotationModifier, x * -1, y * -1 * CounterRotationModifier} });
}

// Scale the actor based on argument scale
void AAcadentalTestPawn::Scale(float scale) {
	if (HitActor == NULL) {
		return;
	}

	HitActor->SetActorScale3D(HitActor->GetActorScale3D().operator+(FVector3d {scale, scale, scale}));
}

FString AAcadentalTestPawn::LogActionToFile(FString Action, FString ObjectName, FString Data) {
	FString directory = FPaths::GameSourceDir();
	IPlatformFile& file = FPlatformFileManager::Get().GetPlatformFile();

	if (file.CreateDirectory(*directory)) {
		FString myFile = directory + "/" + Filename;
		FFileHelper::SaveStringToFile(Action + " " + ObjectName + " " + Data + "\n", *myFile, FFileHelper::EEncodingOptions::AutoDetect, &IFileManager::Get(), EFileWrite::FILEWRITE_Append);
		return myFile;
	}
	return "failed";
}