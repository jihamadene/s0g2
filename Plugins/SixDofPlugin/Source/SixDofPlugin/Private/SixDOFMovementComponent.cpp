// Copyright 1998-2015 Epic Games, Inc. All Rights Reserved.


#include "SixDofPluginPrivatePCH.h"

USixDOFMovementComponent::USixDOFMovementComponent()
{
	// Initialization

	//Custom Movement Component


	CameraMaxPitch = 89.0f;
	CameraMinPitch = -75.0f;

	CustomMovementMode = ECustomMovementMode::ECMM_Fly;
	SpeedBoostMode = ESpeedBoostMode::ESBM_Both;
	bCanJump = true;
	JumpVelocity = 250.0f;
	WalkSpeed = 1000.0f;
	WalkAcceleration = 2048.0f;
	WalkDeceleration = 2048.0f;
	JumpDeceleration = 0.5f;
	FlySpeed = 1000.0f;
	FlyAcceleration = 2048.0f;
	FlyDeceleration = 2048.0f;
	MaxWalkableAngle = 45.0f;
	WalkSpeedBoostMultiplier = 1.5f;
	FlySpeedBoostMultiplier = 1.5f;
	AirControlRatio = 0.5f;
	



	OrientationToSurfaceInfo = FOrientationToSurfaceInfo();
	SurfaceGravityInfo = FSurfaceGravityInfo();
	SurfaceHitToleranceDistance = 30.0f;
	bCanStepOnOtherPawns = false;
	bCanStepOnPhysicsObjects = false;
	SurfaceDetectionTraceShape = ETraceShape::ETS_Sphere;
	SurfaceRestitution = -20.0f;
	SurfaceFriction = 0.5f;
	TraceChannel = ECollisionChannel::ECC_Pawn;
	TraceShapeScale = 0.85f;

	DebugDrawType = EDrawDebugTrace::ForOneFrame;
	bShowPawnCapsule = true;
	bShowGizmos = true;

	bEnablePhysicsInteraction = false;
	bCanPushOtherPawns = false;
	HitForceFactor = 0.25f;
	bEnablePhysicsInteraction = true;
	bAllowDownwardForce = false;

	bDebugIsEnabled = false;

	// Floating Pawn Movement

	MaxSpeed = FlySpeed;
	Acceleration = FlyAcceleration;
	Deceleration = FlyDeceleration;

	// Private Variables
	TimeInAir = 0.0f;
	bIsInAir = false;


	// Networking

	bReplicates = true;

}

// Initializes the component
void USixDOFMovementComponent::InitializeComponent()
{
	Super::InitializeComponent();

	
	CapsuleComponent = Cast<UCapsuleComponent>(UpdatedComponent);

	if (CapsuleComponent == nullptr) 
	{ 
		return; 
	}

	CurrentCapsuleQuat = UpdatedComponent->GetComponentQuat();
	

	TimeInAir = 0.0f;
	bIsInAir = false;
	SetCustomMovementMode(CustomMovementMode, false);

	if (CustomMovementMode == ECustomMovementMode::ECMM_Walk)
	{
		SurfaceGravityInfo.GravityDirection = -CapsuleComponent->GetUpVector();
	}

		UPhysicalMaterial* PhysMat =NewObject<UPhysicalMaterial>(this, (TEXT("SixDOF_Pawn_PM")));
		
		PhysMat->Restitution = SurfaceRestitution;
		PhysMat->Friction = SurfaceFriction;

		CapsuleComponent->SetPhysMaterialOverride(PhysMat);
}


// Called every frame
void USixDOFMovementComponent::TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction *ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// Stop if CapsuleComponet is invalid
	if (CapsuleComponent == nullptr)
	{
		return;
	}

	WalkPhysicsUpdate(DeltaTime);
}

void USixDOFMovementComponent::WalkPhysicsUpdate(float DeltaTime)
{

	/* Trace Test & Result  *********************************************************************************************************************************************/
	const EDrawDebugTrace::Type DrawDebugType = bDebugIsEnabled ? DebugDrawType : EDrawDebugTrace::None;
	const ECollisionChannel CollisionChannel = CapsuleComponent->GetCollisionObjectType();
	const FVector TraceStart = CapsuleComponent->GetComponentLocation();
	const float CapsuleHalfHeight = CapsuleComponent->GetScaledCapsuleHalfHeight();
	float ShapeRadius = CapsuleComponent->GetScaledCapsuleRadius() * 0.99f;
	FVector TraceEnd = TraceStart - CapsuleComponent->GetUpVector()* (CapsuleHalfHeight - ShapeRadius + SurfaceHitToleranceDistance + 1.0f);
	FHitResult HitResult;
	TArray<AActor*> ActorsToIgnore;


	ShapeRadius = CapsuleComponent->GetScaledCapsuleRadius() * TraceShapeScale;
	TraceEnd = TraceStart - CapsuleComponent->GetUpVector()* (CapsuleHalfHeight + SurfaceHitToleranceDistance + 1.41f);

	if (SurfaceDetectionTraceShape == ETraceShape::ETS_Line)
	{
		UKismetSystemLibrary::LineTraceSingle_NEW(this, TraceStart, TraceEnd,
			UEngineTypes::ConvertToTraceType(TraceChannel), true, ActorsToIgnore, DrawDebugType, HitResult, true);
	}
	else if (SurfaceDetectionTraceShape == ETraceShape::ETS_Sphere)
	{
		TraceEnd += CapsuleComponent->GetUpVector() * ShapeRadius;
		UKismetSystemLibrary::SphereTraceSingle_NEW(this, TraceStart, TraceEnd, ShapeRadius,
			UEngineTypes::ConvertToTraceType(TraceChannel), true, ActorsToIgnore, DrawDebugType, HitResult, true);
	}
	else
	{
		TraceEnd += CapsuleComponent->GetUpVector() * ShapeRadius;
		UKismetSystemLibrary::BoxTraceSingle(this, TraceStart, TraceEnd, FVector(1, 1, 1)*ShapeRadius, CapsuleComponent->GetComponentRotation(),
			UEngineTypes::ConvertToTraceType(TraceChannel), true, ActorsToIgnore, DrawDebugType, HitResult, true);
	}


	TracedSurface = HitResult;
	const bool bBolckingHit = TracedSurface.IsValidBlockingHit();
	bIsInAir = true;
	bIsOnWalkableSurface = false;

	if (bBolckingHit)
	{

		const bool bHitIsSimPhys = TracedSurface.GetComponent()->IsSimulatingPhysics();
		const bool bHitIsPawn = Cast<APawn>(TracedSurface.GetActor()) != nullptr;

		const bool Condition1 = (bCanStepOnPhysicsObjects && bHitIsSimPhys) || (bCanStepOnOtherPawns && bHitIsPawn);
		const bool Condition2 = !bHitIsSimPhys;

		if (Condition1 || Condition2)
		{
			bIsOnWalkableSurface = true;
		}

	}


	/* Walk Mode ********************************************************************************************************************************************************/


	if (CustomMovementMode == ECustomMovementMode::ECMM_Walk)
	{
		if (bIsOnWalkableSurface)
		{
			SurfaceGravityInfo.GravityDirection = -TracedSurface.ImpactNormal;
			bIsInAir = false;
		}


		/** Variables definition & initialization */
		const FVector CurrentGravityDirection = SurfaceGravityInfo.GravityDirection;
		const float CurrentGravityPower = SurfaceGravityInfo.GravityPower * SurfaceGravityInfo.GravityScale;
		const FVector GravityForce = CurrentGravityDirection.GetSafeNormal() * CurrentGravityPower;

		const float InterpSpeed = OrientationToSurfaceInfo.RotationSpeed;
		const bool bOrientationIsInstant = OrientationToSurfaceInfo.bIsInstantOrientation;


		/* Update Rotation : Orient Capsule's up vector to have the same direction as -gravityDirection */
		CapsuleRotationUpdate(DeltaTime, -CurrentGravityDirection, bOrientationIsInstant, InterpSpeed);

		/* Apply Gravity*/
		ApplySurfaceGravity(GravityForce, SurfaceGravityInfo.bForceSubStepping, true);

	}

	TimeInAir = bIsInAir ? TimeInAir + DeltaTime : 0.0f;
}


void USixDOFMovementComponent::CapsuleRotationUpdate(float DeltaTime, const FVector& TargetUpVector, bool bInstantRot, float RotationSpeed)
{
	const FVector CapsuleUp = CapsuleComponent->GetUpVector();
	const FQuat DeltaQuat = FQuat::FindBetween(CapsuleUp, TargetUpVector);
	const FQuat TargetQuat = DeltaQuat * CapsuleComponent->GetComponentRotation().Quaternion();

	CurrentCapsuleQuat = bInstantRot ? TargetQuat : FQuat::Slerp(CapsuleComponent->GetComponentQuat(), TargetQuat, DeltaTime * RotationSpeed);

	FHitResult Hit(1.0f);
	SafeMoveUpdatedComponent(FVector::ZeroVector, CurrentCapsuleQuat, true, Hit);
//	CapsuleComponent->SetWorldRotation(CurrentCapsuleQuat);

}


void USixDOFMovementComponent::ApplySurfaceGravity(const FVector& Force, bool bAllowSubstepping, bool bAccelChange)
{
	CapsuleComponent->GetBodyInstance()->AddForce(Force, bAllowSubstepping, bAccelChange);
}


void USixDOFMovementComponent::DoJump()
{
	if ((CustomMovementMode == ECustomMovementMode::ECMM_Fly) || (bCanJump == false))
	{
		return;
	}

	float TargetJumpHeight;
	FVector JumpImpulse;

	if (IsMovingOnGround())
	{
		TargetJumpHeight = JumpVelocity +  CapsuleComponent->GetScaledCapsuleHalfHeight();
		JumpImpulse = CapsuleComponent->GetUpVector() * (FMath::Sqrt((TargetJumpHeight *(2.f * GetGravityPower() + WalkDeceleration))));
		
		Velocity += JumpImpulse;
		bIsJumping = true;
	}
	

}


void USixDOFMovementComponent::DoSpeedBoost()
{
	if (SpeedBoostMode == ESpeedBoostMode::ESBM_None)
	{
		DoStopSpeedBoost();
		return;
	}
	else
	{

		 float SpeedBoostMultiplier = 1.0f;
		 float Speed = MaxSpeed;

		if (SpeedBoostMode == ESpeedBoostMode::ESBM_Both)
		{
			SpeedBoostMultiplier = (CustomMovementMode == ECustomMovementMode::ECMM_Walk) ? WalkSpeedBoostMultiplier : FlySpeedBoostMultiplier;
			Speed = (CustomMovementMode == ECustomMovementMode::ECMM_Walk) ? WalkSpeed : FlySpeed;
			MaxSpeed = SpeedBoostMultiplier * Speed;
		}

		else if (SpeedBoostMode == ESpeedBoostMode::ESBM_Fly && CustomMovementMode == ECustomMovementMode::ECMM_Fly)
		{
			SpeedBoostMultiplier = FlySpeedBoostMultiplier;
			Speed = FlySpeed;
		}

		else if (SpeedBoostMode == ESpeedBoostMode::ESBM_Walk && CustomMovementMode == ECustomMovementMode::ECMM_Walk)
		{
			SpeedBoostMultiplier = WalkSpeedBoostMultiplier;
			Speed = WalkSpeed;
		}

		MaxSpeed = SpeedBoostMultiplier * Speed;
		bIsSpeedBoost = true;
	}

}


void USixDOFMovementComponent::DoStopSpeedBoost()
{
	const float Speed = (CustomMovementMode == ECustomMovementMode::ECMM_Walk) ? WalkSpeed : FlySpeed;
	MaxSpeed = Speed;
	bIsSpeedBoost = false;
}


void USixDOFMovementComponent::StopMovementImmediately()
{
	Super::StopMovementImmediately();

	const FVector ZeroVelocity = FVector(0.f, 0.f, 0.f);
	Velocity = ZeroVelocity;

	if (CapsuleComponent != NULL)
	{
		CapsuleComponent->SetPhysicsLinearVelocity(ZeroVelocity);
	}
}


void USixDOFMovementComponent::CapsuleHited(class UPrimitiveComponent* MyComp, class AActor* Other, class UPrimitiveComponent* OtherComp, bool bSelfMoved, FVector HitLocation, FVector HitNormal, FVector NormalImpulse, const FHitResult& Hit)
{


	const float OnGroundHitDot = FVector::DotProduct(HitNormal, CapsuleComponent->GetUpVector());

	if (OnGroundHitDot > 0.75f)
	{
		FVector HorizontalVelocity = CapsuleComponent->GetComponentTransform().InverseTransformVector(Velocity);


		if (CustomMovementMode == ECustomMovementMode::ECMM_Walk && FMath::Abs(Velocity.Z) > SMALL_NUMBER)
		{
			HorizontalVelocity.Z = 0.0f;
			Velocity = CapsuleComponent->GetComponentTransform().TransformVector(HorizontalVelocity);
		}

		bIsJumping = false;
	}

	CapsuleHitResult = Hit;

	if (!bEnablePhysicsInteraction)
	{
		return;
	}

	if (OtherComp != NULL && OtherComp->IsAnySimulatingPhysics())
	{
		const FVector OtherLoc = OtherComp->GetComponentLocation();
		const FVector Loc = CapsuleComponent->GetComponentLocation();
		FVector ImpulseDir = (OtherLoc - Loc).GetSafeNormal();
		ImpulseDir = FVector::VectorPlaneProject(ImpulseDir, CapsuleComponent->GetUpVector());
		ImpulseDir = (ImpulseDir + GetMovementVelocity().GetSafeNormal()) * 0.5f;
		ImpulseDir.Normalize();


		float TouchForceFactorModified = HitForceFactor;

		if (bHitForceScaledToMass)
		{
			FBodyInstance* BI = OtherComp->GetBodyInstance();
			TouchForceFactorModified *= BI ? BI->GetBodyMass() : 1.0f;
		}

		float ImpulseStrength = GetMovementVelocity().Size() * TouchForceFactorModified;

		FVector Impulse = ImpulseDir * ImpulseStrength;
		float dot = FVector::DotProduct(HitNormal, CapsuleComponent->GetUpVector());

		if (dot > 0.99f && !bAllowDownwardForce)
		{
			return;
		}

		OtherComp->AddImpulseAtLocation(Impulse, HitLocation);
	}
}


bool USixDOFMovementComponent::IsMovingOnGround() const
{
	return !bIsInAir;
}


bool USixDOFMovementComponent::IsFalling() const 
{
	return bIsInAir;
}


FVector USixDOFMovementComponent::GetFallingVelocity() const
{
	return CapsuleComponent->GetComponentVelocity().ProjectOnTo(CapsuleComponent->GetUpVector());
}


FVector USixDOFMovementComponent::GetMovementVelocity() const
{
	const FVector UpVector = CapsuleComponent ? CapsuleComponent->GetUpVector() : FVector::UpVector;
	return FVector::VectorPlaneProject(Velocity, UpVector);
}


float USixDOFMovementComponent::GetFallingSpeed() const
{
	const float Direction = FVector::DotProduct(CapsuleComponent->GetUpVector(), GetFallingVelocity());

	return GetFallingVelocity().Size() * FMath::Sign(Direction);
}


float USixDOFMovementComponent::GetCurrentMovementSpeed() const
{
	return Velocity.Size();
}


float USixDOFMovementComponent::GetTotalTimeInAir() const
{
	return TimeInAir;
}


void USixDOFMovementComponent::EnableDebuging()
{
	bDebugIsEnabled = true;
}


void USixDOFMovementComponent::DisableDebuging()
{
	bDebugIsEnabled = false;
}


void USixDOFMovementComponent::SetComponentOwner(class ASixDOFPawn* Owner)
{
	ComponentOwner = Owner;
}


void USixDOFMovementComponent::SetCustomMovementMode(TEnumAsByte<ECustomMovementMode::Type> NewMovementMode, bool bMaintainCameraRotationOnFlySwitch)
{
	switch (NewMovementMode)
	{
	case ECustomMovementMode::ECMM_Fly:
	{
		CapsuleComponent->SetLinearDamping(10.0f);
		MaxSpeed = FlySpeed;
		Acceleration = FlyAcceleration;
		Deceleration = FlyDeceleration;

		if (bMaintainCameraRotationOnFlySwitch)
		{
			const FQuat SpringArmQuat = ComponentOwner->GetSpringArm()->GetComponentQuat();
			CapsuleComponent->SetWorldRotation(SpringArmQuat);
		}

		bIsJumping = false;

		break;
	}

	case ECustomMovementMode::ECMM_Walk:
	{

		CapsuleComponent->SetLinearDamping(0.5f);
		MaxSpeed = WalkSpeed;
		Acceleration = WalkAcceleration;
		Deceleration = WalkDeceleration;

		break;
	}
	}

	CustomMovementMode = NewMovementMode;
}

void USixDOFMovementComponent::SetMaxWalkableAngle(float NewAngle)
{
	MaxWalkableAngle = FMath::Clamp<float>(NewAngle, 0.0f, 90.0f);
}

void USixDOFMovementComponent::SetTraceShapeScale(float NewScale)
{
	TraceShapeScale = FMath::Clamp<float>(NewScale, 0.0f, 0.99f);
}

void USixDOFMovementComponent::SetCameraMinMaxPitch(float NewMin, float NewMax)
{
	CameraMinPitch = FMath::Clamp<float>(NewMin, -89.0f, 0.0f);
	CameraMaxPitch = FMath::Clamp<float>(NewMax, 0.0f, 90.0f);
}

bool USixDOFMovementComponent::IsSpeedBoost() const
{
	return bIsSpeedBoost;
}

bool USixDOFMovementComponent::IsJumping() const
{
	return bIsJumping;
}

float USixDOFMovementComponent::GetGravityPower() const
{
	const bool bIsWalking = (CustomMovementMode == ECustomMovementMode::ECMM_Walk);

	return bIsWalking ? SurfaceGravityInfo.GravityPower * SurfaceGravityInfo.GravityScale : 0.0f;
}


bool USixDOFMovementComponent::IsOnWalkableSurface() const
{
	return bIsOnWalkableSurface;
}

bool USixDOFMovementComponent::CanWalkOnSurface() const
{
	if (CapsuleComponent && IsOnWalkableSurface())
	{
		const float _Dot = FVector::DotProduct(TracedSurface.ImpactNormal, CapsuleComponent->GetUpVector());
		const float _Angle = FMath::Acos(_Dot) *  (180.0f / PI);

		return (_Angle >= 0.0f && _Angle < MaxWalkableAngle);
	}

	return false;
}

FVector USixDOFMovementComponent::GetGravityDirection() const
{
	const bool bIsWalking = (CustomMovementMode == ECustomMovementMode::ECMM_Walk);

	return bIsWalking ? SurfaceGravityInfo.GravityDirection : FVector(0.0f, 0.0, 0.0f);
}