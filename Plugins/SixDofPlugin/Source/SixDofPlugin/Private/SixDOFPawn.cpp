// Copyright 1998-2015 Epic Games, Inc. All Rights Reserved.

#include "SixDofPluginPrivatePCH.h"

// Sets default values
ASixDOFPawn::ASixDOFPawn(const FObjectInitializer& ObjectInitializer) :Super(ObjectInitializer)
{
	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	const float CapsuleHalfHeight = 96.0f;
	const float CapsuleRadius = 42.0f;

	CapsuleComponent = CreateDefaultSubobject<UCapsuleComponent>(TEXT("CollisionCapsule0"));
	if (CapsuleComponent)
	{
		CapsuleComponent->InitCapsuleSize(CapsuleRadius, CapsuleHalfHeight);
		CapsuleComponent->SetCollisionProfileName(TEXT("Pawn"));
		CapsuleComponent->SetSimulatePhysics(true);
		CapsuleComponent->SetEnableGravity(false);
		RootComponent = CapsuleComponent;
		CapsuleComponent->GetBodyInstance()->COMNudge = FVector(0.0f, 0.0f, -96.0f);
		CapsuleComponent->SetLinearDamping(0.15f);
		CapsuleComponent->SetAngularDamping(100.0f);
		CapsuleComponent->SetNotifyRigidBodyCollision(true);
	}


	SpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArm0"));
	if (SpringArm)
	{
		SpringArm->TargetArmLength = -40.0f;
		SpringArm->bDoCollisionTest = true;
		SpringArm->ProbeSize = 15.0f;
		SpringArm->ProbeChannel = ECollisionChannel::ECC_Camera;
		SpringArm->bUsePawnControlRotation = false;
		SpringArm->bInheritPitch = true;
		SpringArm->bInheritYaw = true;
		SpringArm->bInheritRoll = true;
		SpringArm->AttachParent = CapsuleComponent;
		SpringArm->SetRelativeLocation(FVector::UpVector * CapsuleHalfHeight);
	}

	Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("MainCamera0"));
	if (Camera)
	{
		Camera->FieldOfView = 90.0f;
		Camera->AttachTo(SpringArm, SpringArm->SocketName);
	}

	

	PawnMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("PawnMesh0"));
	if (PawnMesh)
	{
		PawnMesh->MeshComponentUpdateFlag = EMeshComponentUpdateFlag::AlwaysTickPose;
		PawnMesh->bCastDynamicShadow = true;
		PawnMesh->bAffectDynamicIndirectLighting = true;
		PawnMesh->PrimaryComponentTick.TickGroup = TG_PrePhysics;
		PawnMesh->SetCollisionProfileName(TEXT("CharacterMesh"));
		PawnMesh->bGenerateOverlapEvents = false;
		PawnMesh->SetNotifyRigidBodyCollision(false);
		PawnMesh->AttachParent = CapsuleComponent;
		PawnMesh->SetRelativeLocationAndRotation(-FVector::UpVector * CapsuleHalfHeight, FRotator(0.0f, -90.0f, 0.0f));
	}

	MovementComponent = CreateDefaultSubobject<USixDOFMovementComponent>(TEXT("MovementComponent0"));
	if (MovementComponent)
	{
		MovementComponent->SetUpdatedComponent(CapsuleComponent);
		MovementComponent->SetComponentOwner(this);
	}

	GizmoRootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("GizmoRootComponent0"));

	

	if (GizmoRootComponent)
	{
		GizmoRootComponent->AttachParent = CapsuleComponent;

		ForwardArrowComponent = CreateDefaultSubobject<UArrowComponent>(TEXT("ForwardArrowComponent0"));
		if (ForwardArrowComponent)
		{
			ForwardArrowComponent->ArrowColor = FColor::Red;
			ForwardArrowComponent->bTreatAsASprite = true;
			ForwardArrowComponent->AttachParent = GizmoRootComponent;
			ForwardArrowComponent->bIsScreenSizeScaled = true;
			ForwardArrowComponent->SetRelativeRotation(FRotator(0.0f, 0.0f, 0.0f));
		}

		RightArrowComponent = CreateDefaultSubobject<UArrowComponent>(TEXT("RightArrowComponent0"));
		if (RightArrowComponent)
		{
			RightArrowComponent->ArrowColor = FColor::Green;
			RightArrowComponent->bTreatAsASprite = true;
			RightArrowComponent->AttachParent = GizmoRootComponent;
			RightArrowComponent->bIsScreenSizeScaled = true;
			RightArrowComponent->SetRelativeRotation(FRotator(0.0f, 90.0f, 0.0f));

		}

		UpArrowComponent = CreateDefaultSubobject<UArrowComponent>(TEXT("UpArrowtComponent0"));
		if (UpArrowComponent)
		{
			UpArrowComponent->ArrowColor = FColor::Blue;
			UpArrowComponent->bTreatAsASprite = true;
			UpArrowComponent->AttachParent = GizmoRootComponent;
			UpArrowComponent->bIsScreenSizeScaled = true;
			UpArrowComponent->SetRelativeRotation(FRotator(90.0f, 0.0f, 0.0f));

		}

		GizmoRootComponent->SetVisibility(true, true);
		GizmoRootComponent->SetHiddenInGame(true, true);
	}
	

	// Networking
	bReplicateMovement = true;
	bReplicates = true;
}

void ASixDOFPawn::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	if (!IsPendingKill())
	{
		if (PawnMesh)
		{

			// force animation tick after movement component updates
			if (PawnMesh->PrimaryComponentTick.bCanEverTick && MovementComponent)
			{
				PawnMesh->PrimaryComponentTick.AddPrerequisite(MovementComponent, MovementComponent->PrimaryComponentTick);
			}
		}
	}


}

void ASixDOFPawn::BeginPlay()
{
	Super::BeginPlay();
}

// Called every frame
void ASixDOFPawn::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ASixDOFPawn::NotifyHit(class UPrimitiveComponent* MyComp, class AActor* Other, class UPrimitiveComponent* OtherComp, bool bSelfMoved, FVector HitLocation, FVector HitNormal, FVector NormalImpulse, const FHitResult& Hit)
{
	Super::NotifyHit(MyComp, Other, OtherComp, bSelfMoved, HitLocation, HitNormal, NormalImpulse, Hit);

	if (MovementComponent)
	{
		MovementComponent->CapsuleHited(MyComp, Other, OtherComp, bSelfMoved, HitLocation, HitNormal, NormalImpulse, Hit);
	}
}

void ASixDOFPawn::AddForwardMovementInput(float ScaleValue /*= 1.0f*/, bool bForce /*= false*/)
{
	if (MovementComponent && CapsuleComponent)
	{
		const float ControlValue = (MovementComponent->IsFalling() && MovementComponent->CustomMovementMode == ECustomMovementMode::ECMM_Walk) ?
			ScaleValue * MovementComponent->AirControlRatio : ScaleValue;
		AddMovementInput(CapsuleComponent->GetForwardVector(), ControlValue, bForce);
	}
}


void ASixDOFPawn::AddRightMovementInput(float ScaleValue /*= 1.0f*/, bool bForce /*= false*/)
{
	if (MovementComponent && CapsuleComponent)
	{
		const float ControlValue = (MovementComponent->IsFalling() && MovementComponent->CustomMovementMode == ECustomMovementMode::ECMM_Walk) ?
			ScaleValue * MovementComponent->AirControlRatio : ScaleValue;
		AddMovementInput(CapsuleComponent->GetRightVector(), ControlValue, bForce);
	}
}


void ASixDOFPawn::AddUpMovementInput(float ScaleValue /*= 1.0f*/, bool bForce /*= false*/)
{
	if (MovementComponent && CapsuleComponent)
	{
		if (MovementComponent->CustomMovementMode == ECustomMovementMode::ECMM_Fly)
		{
			const float ControlValue = MovementComponent->IsMovingOnGround() ? ScaleValue : ScaleValue * MovementComponent->AirControlRatio;
			AddMovementInput(CapsuleComponent->GetUpVector(), ControlValue, bForce);
		}
	}
}



void ASixDOFPawn::AddPawnPitchInput(float UpdateRate /*= 1.0f*/, float ScaleValue /*= 0.0f */, bool bEnableCameraPitchControlOnWalk/*=true*/)
{
	if (MovementComponent && CapsuleComponent && SpringArm)
	{
		if (MovementComponent->CustomMovementMode == ECustomMovementMode::ECMM_Fly)
		{
			SpringArm->SetRelativeRotation(FRotator::ZeroRotator);
			CapsuleComponent->AddLocalRotation(FRotator(ScaleValue * UpdateRate, 0.0f, 0.0f));
		}
		else if (MovementComponent->CustomMovementMode == ECustomMovementMode::ECMM_Walk && bEnableCameraPitchControlOnWalk)
		{
			const float CameraPitchMin = MovementComponent->CameraMinPitch;
			const float	CameraPitchMax = MovementComponent->CameraMaxPitch;
			FRotator CameraRelativeRot = SpringArm->RelativeRotation;
			float CameraNewPitch = FMath::ClampAngle(CameraRelativeRot.Pitch + ScaleValue * UpdateRate, CameraPitchMin, CameraPitchMax);
			CameraRelativeRot.Pitch = CameraNewPitch;
			SpringArm->SetRelativeRotation(CameraRelativeRot);
		}
	}
}

void ASixDOFPawn::AddPawnYawInput(float UpdateRate /*= 1.0f*/, float ScaleValue /*= 0.0f*/)
{
	if (CapsuleComponent)
	{
		CapsuleComponent->AddLocalRotation(FRotator(0.0f, ScaleValue * UpdateRate, 0.0f));
	}
}


void ASixDOFPawn::AddPawnRollInput(float UpdateRate, float ScaleValue)
{
	if (MovementComponent && CapsuleComponent)
	{
		if (MovementComponent->CustomMovementMode == ECustomMovementMode::ECMM_Fly)
		{
			CapsuleComponent->AddLocalRotation(FRotator(0.0f, 0.0f, ScaleValue * UpdateRate));
		}
	}
}

void ASixDOFPawn::PawnLaunch(FVector LaunchVelocity, bool bOverrideCurrentVelocity)
{
	if (MovementComponent)
	{
		FVector FinalVel = LaunchVelocity;
		const FVector CurrentVelocity = MovementComponent->Velocity;

		if (!bOverrideCurrentVelocity)
		{
			FinalVel += CurrentVelocity;
		}

		MovementComponent->Velocity = FinalVel;
	}
}

void ASixDOFPawn::PawnLaunchToTarget(class AActor* Target, float LaunchPower, bool bOverrideCurrentVelocity)
{
	if (MovementComponent != nullptr && Target != nullptr)
	{
		FVector LaunchDirection = (Target->GetActorLocation() - GetActorLocation()).GetSafeNormal();
		FVector FinalVel = LaunchDirection * LaunchPower;
		const FVector CurrentVelocity = MovementComponent->Velocity;

		if (!bOverrideCurrentVelocity)
		{
			FinalVel += CurrentVelocity;
		}

		MovementComponent->Velocity = FinalVel;
	}
}

void ASixDOFPawn::Jump()
{
	if (MovementComponent)
	{
		MovementComponent->DoJump();
	}
}

void ASixDOFPawn::SpeedBoost()
{
	if (MovementComponent)
	{
		MovementComponent->DoSpeedBoost();
	}
}

void ASixDOFPawn::StopSpeedBoost()
{
	if (MovementComponent)
	{
		MovementComponent->DoStopSpeedBoost();
	}
}

void ASixDOFPawn::EnableDebugging()
{
	if (CapsuleComponent) { CapsuleComponent->SetHiddenInGame(false); }
	if (GizmoRootComponent) { GizmoRootComponent->SetHiddenInGame(false, true); }
	if (MovementComponent) { MovementComponent->EnableDebuging(); }
}

void ASixDOFPawn::DisableDebugging()
{
	if (MovementComponent)
	{
		if (CapsuleComponent) { CapsuleComponent->SetHiddenInGame(MovementComponent->bShowPawnCapsule); }
		if (GizmoRootComponent) { GizmoRootComponent->SetHiddenInGame(MovementComponent->bShowGizmos, true); }
		MovementComponent->DisableDebuging();
	}
}
