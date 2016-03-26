// Copyright 1998-2015 Epic Games, Inc. All Rights Reserved.



#pragma once
#include "Kismet/KismetSystemLibrary.h"
#include "SixDOFMovementComponent.generated.h"





/** Struct to hold information about the "Gravity Type" . */
USTRUCT(BlueprintType)
struct  SIXDOFPLUGIN_API FSurfaceGravityInfo
{
	GENERATED_BODY()

public:

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		float GravityScale;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		float GravityPower;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FVector GravityDirection;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		bool bForceSubStepping;

	FSurfaceGravityInfo()
	{
		GravityScale = 1.0f;
		GravityPower = 980.0f;
		GravityDirection = FVector(0.0f, 0.0f, -1.0f);
		bForceSubStepping = true;
	}

	FSurfaceGravityInfo(float NewGravityScale, float NewGravityPower, FVector NewGravityDirection, bool bShouldUseStepping)
	{
		GravityScale = NewGravityScale;
		GravityPower = NewGravityPower;
		GravityDirection = NewGravityDirection;
		bForceSubStepping = bShouldUseStepping;
	}

};


UENUM(BlueprintType)
namespace ETraceShape
{
	enum  Type
	{
		ETS_Sphere	UMETA(DisplayName = "Sphere"),
		ETS_Box	UMETA(DisplayName = "Box"),
		ETS_Line 	UMETA(DisplayName = "Line")
	};
}

UENUM(BlueprintType)
namespace ESpeedBoostMode
{
	enum  Type
	{
		ESBM_None	UMETA(DisplayName = "None"),
		ESBM_Fly	UMETA(DisplayName = "On Fly Only"),
		ESBM_Walk	UMETA(DisplayName = "On Walk Only"),
		ESBM_Both	UMETA(DisplayName = "Both"),	
	};
}

UENUM(BlueprintType)
namespace ECustomMovementMode
{
	enum  Type
	{
		ECMM_Fly	UMETA(DisplayName = "Fly"),
		ECMM_Walk 	UMETA(DisplayName = "Walk")
	};
}



USTRUCT(BlueprintType)
struct FOrientationToSurfaceInfo
{
	GENERATED_BODY()
public:

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		bool bIsInstantOrientation;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		float RotationSpeed;

	FOrientationToSurfaceInfo()
	{
		bIsInstantOrientation = false;
		RotationSpeed = 5.0f;
	}
	FOrientationToSurfaceInfo(bool _isInstant, float _interpSpeed)
	{
		bIsInstantOrientation = _isInstant;
		RotationSpeed = _interpSpeed;
	}
};



UCLASS()
class  SIXDOFPLUGIN_API USixDOFMovementComponent : public UFloatingPawnMovement
{
	GENERATED_BODY()

public:



	/**
	* Default UObject constructor.
	*/
	USixDOFMovementComponent();

	//Begin UActorComponent Interface
	virtual void InitializeComponent() override;
	virtual void TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction *ThisTickFunction) override;
	//End UActorComponent Interface


	//BEGIN UMovementComponent Interface
	virtual bool IsMovingOnGround() const override;
	virtual bool IsFalling() const override;
	virtual void StopMovementImmediately() override;
	//END UMovementComponent Interface

	/**
	* Called when the collision capsule touches another primitive component
	* Handles physics interaction logic	*/
	UFUNCTION()
		virtual void CapsuleHited(class UPrimitiveComponent* MyComp,
	class AActor* Other,
	class UPrimitiveComponent* OtherComp,
	bool bSelfMoved,
	FVector HitLocation,
	FVector HitNormal, FVector
	NormalImpulse,
	const FHitResult& Hit);

	virtual void CapsuleRotationUpdate(float DeltaTime, const FVector& TargetUpVector, bool bInstantRot, float RotationSpeed);
	virtual void ApplySurfaceGravity(const FVector& Force, bool bAllowSubstepping, bool bAccelChange);
	virtual void WalkPhysicsUpdate(float DeltaTime);
	virtual void DoJump();
	virtual void DoSpeedBoost();
	virtual void DoStopSpeedBoost();
	virtual void EnableDebuging();
	virtual void DisableDebuging();

	/** */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Six DOF Movement Component : Camera Settings", meta = (ClampMin = "-89.0", ClampMax = "0.0", UIMin = "-89.0", UIMax = "0.0"))
		float CameraMinPitch;

	/**  */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Six DOF Movement Component : Camera Settings", meta = (ClampMin = "0.0", ClampMax = "89.0", UIMin = "0.0", UIMax = "89.0"))
		float CameraMaxPitch;

	/***/
	UPROPERTY(Category = "Six DOF Movement Component : Movement Settings", EditAnywhere, BlueprintReadOnly)
		TEnumAsByte<ECustomMovementMode::Type> CustomMovementMode;

	/***/
	UPROPERTY(Category = "Six DOF Movement Component : Movement Settings", EditAnywhere, BlueprintReadOnly)
		TEnumAsByte<ESpeedBoostMode::Type> SpeedBoostMode;

	/**  */
	UPROPERTY(Category = "Six DOF Movement Component : Movement Settings", EditAnywhere, BlueprintReadWrite, DisplayName = "Can Jump")
		bool bCanJump;

	/**  */
	UPROPERTY(Category = "Six DOF Movement Component : Movement Settings", EditAnywhere, BlueprintReadWrite, meta = (editcondition = "bCanJump"))
		float JumpVelocity;

	UPROPERTY(Category = "Six DOF Movement Component : Movement Settings", EditAnywhere, BlueprintReadWrite)
		float WalkSpeed;

	UPROPERTY(Category = "Six DOF Movement Component : Movement Settings", EditAnywhere, BlueprintReadWrite)
		float WalkAcceleration;

	/**  */
	UPROPERTY(Category = "Six DOF Movement Component : Movement Settings", EditAnywhere, BlueprintReadWrite, meta = (ClampMin = "0", UIMin = "0"))
		float WalkSpeedBoostMultiplier;

	UPROPERTY(Category = "Six DOF Movement Component : Movement Settings", EditAnywhere, BlueprintReadWrite)
		float WalkDeceleration;

	UPROPERTY(Category = "Six DOF Movement Component : Movement Settings", EditAnywhere, BlueprintReadWrite)
		float JumpDeceleration;

	UPROPERTY(Category = "Six DOF Movement Component : Movement Settings", EditAnywhere, BlueprintReadWrite)
		float FlySpeed;

	/**  */
	UPROPERTY(Category = "Six DOF Movement Component : Movement Settings", EditAnywhere, BlueprintReadWrite, meta = (ClampMin = "0", UIMin = "0"))
		float FlySpeedBoostMultiplier;

	UPROPERTY(Category = "Six DOF Movement Component : Movement Settings", EditAnywhere, BlueprintReadWrite)
		float FlyAcceleration;

	UPROPERTY(Category = "Six DOF Movement Component : Movement Settings", EditAnywhere, BlueprintReadWrite)
		float FlyDeceleration;

	/***/
	UPROPERTY(Category = "Six DOF Movement Component : Movement Settings", EditAnywhere, BlueprintReadWrite, meta = (ClampMin = "0.0", ClampMax = "1.0", UIMin = "0.0", UIMax = "1.0"))
		float AirControlRatio;


	UPROPERTY(Category = "Six DOF Movement Component : Surface Settings", EditAnywhere, BlueprintReadWrite)
		FSurfaceGravityInfo SurfaceGravityInfo;

	/***/
	UPROPERTY(Category = "Six DOF Movement Component : Surface Settings", EditAnywhere, BlueprintReadWrite)
		FOrientationToSurfaceInfo OrientationToSurfaceInfo;

	/** */
	UPROPERTY(Category = "Six DOF Movement Component : Surface Settings", EditAnywhere, BlueprintReadWrite, meta = (ClampMin = "0", UIMin = "0"))
		float SurfaceHitToleranceDistance;

	/** */
	UPROPERTY(Category = "Six DOF Movement Component : Surface Settings", EditAnywhere, BlueprintReadOnly, meta = (ClampMin = "0.0", ClampMax = "90.0", UIMin = "0.0", UIMax = "90.0"))
		float MaxWalkableAngle;

	/**  */
	UPROPERTY(Category = "Six DOF Movement Component : Surface Settings", EditAnywhere, BlueprintReadWrite, meta = (ClampMin = "0", UIMin = "0"))
		float SurfaceFriction;

	/**  */
	UPROPERTY(Category = "Six DOF Movement Component : Surface Settings", EditAnywhere, BlueprintReadWrite, meta = (ClampMin = "0", UIMin = "0"))
		float SurfaceRestitution;

	/**  */
	UPROPERTY(Category = "Six DOF Movement Component : Surface Settings", EditAnywhere, BlueprintReadWrite, meta = (ClampMin = "0", UIMin = "0"))
		bool bCanStepOnPhysicsObjects;

	/**  */
	UPROPERTY(Category = "Six DOF Movement Component : Surface Settings", EditAnywhere, BlueprintReadWrite, meta = (ClampMin = "0", UIMin = "0"))
		bool bCanStepOnOtherPawns;

	/***/
	UPROPERTY(Category = "Six DOF Movement Component : Surface Settings", EditAnywhere, BlueprintReadWrite, meta = (editcondition = "!bUseCapsuleHit"))
		TEnumAsByte<ETraceShape::Type> SurfaceDetectionTraceShape;

	/***/
	UPROPERTY(Category = "Six DOF Movement Component : Surface Settings", EditAnywhere, BlueprintReadWrite, meta = (editcondition = "!bUseCapsuleHit"))
		TEnumAsByte<ECollisionChannel> TraceChannel;

	/***/
	UPROPERTY(Category = "Six DOF Movement Component : Surface Settings", EditAnywhere, BlueprintReadOnly, meta = (ClampMin = "0.0", ClampMax = "0.99", UIMin = "0.0", UIMax = "0.99"), meta = (editcondition = "!bUseCapsuleHit"))
		float TraceShapeScale;

	/***/
	UPROPERTY(Category = "Six DOF Movement Component : Debug Settings", EditAnywhere, BlueprintReadWrite)
		TEnumAsByte<EDrawDebugTrace::Type> DebugDrawType;

	UPROPERTY(Category = "Six DOF Movement Component : Debug Settings", EditAnywhere, BlueprintReadWrite)
		bool bShowPawnCapsule;

	UPROPERTY(Category = "Six DOF Movement Component : Debug Settings", EditAnywhere, BlueprintReadWrite)
		bool bShowGizmos;

	/**  */
	UPROPERTY(Category = "Six DOF Movement Component : Physics Interaction Settings", EditAnywhere, BlueprintReadWrite)
		bool bEnablePhysicsInteraction;

	/**  */
	UPROPERTY(Category = "Six DOF Movement Component : Physics Interaction Settings", EditAnywhere, BlueprintReadWrite, meta = (editcondition = "bEnablePhysicsInteraction"))
		bool bCanPushOtherPawns;

	/**  */
	UPROPERTY(Category = "Six DOF Movement Component : Physics Interaction Settings", EditAnywhere, BlueprintReadWrite, meta = (editcondition = "bEnablePhysicsInteraction"))
		float HitForceFactor = 0.25f;

	/**  */
	UPROPERTY(Category = "Six DOF Movement Component : Physics Interaction Settings", EditAnywhere, BlueprintReadWrite, meta = (editcondition = "bEnablePhysicsInteraction"))
		bool bHitForceScaledToMass = true;

	/** */
	UPROPERTY(Category = "Six DOF Movement Component : Physics Interaction Settings", EditAnywhere, BlueprintReadWrite, meta = (editcondition = "bEnablePhysicsInteraction"))
		bool bAllowDownwardForce = true;


	/**  */
	UPROPERTY(Category = "Six DOF Movement Component", VisibleInstanceOnly, BlueprintReadOnly)
		FHitResult TracedSurface;

	/** */
	UPROPERTY(Category = "Six DOF Movement Component", VisibleInstanceOnly, BlueprintReadOnly)
		FHitResult CapsuleHitResult;


	/** */
	UFUNCTION(BlueprintCallable, Category = "Pawn|Components|SixDOFMovementComponent")
		void SetComponentOwner(class ASixDOFPawn* Owner);

	/**  */
	UFUNCTION(BlueprintCallable, Category = "Pawn|Components|SixDOFMovementComponent")
		void SetCustomMovementMode(TEnumAsByte<ECustomMovementMode::Type> NewMovementMode, bool bMaintainCameraRotationOnFlySwitch);

	/**  */
	UFUNCTION(BlueprintCallable, Category = "Pawn|Components|SixDOFMovementComponent")
		void SetMaxWalkableAngle(float NewAngle);

	/**  */
	UFUNCTION(BlueprintCallable, Category = "Pawn|Components|SixDOFMovementComponent")
		void SetTraceShapeScale(float NewScale);

	/**  */
	UFUNCTION(BlueprintCallable, Category = "Pawn|Components|SixDOFMovementComponent")
		void SetCameraMinMaxPitch(float NewMin, float NewMax);


	/***/
	UFUNCTION(BlueprintCallable, Category = "Pawn|Components|SixDOFMovementComponent")
		float GetGravityPower() const;

	/** */
	UFUNCTION(BlueprintCallable, Category = "Pawn|Components|SixDOFMovementComponent")
		virtual bool IsOnWalkableSurface() const;

	/**  */
	UFUNCTION(BlueprintCallable, Category = "Pawn|Components|SixDOFMovementComponent")
		virtual bool CanWalkOnSurface() const;

	/** */
	UFUNCTION(BlueprintCallable, Category = "Pawn|Components|SixDOFMovementComponent")
		FVector GetGravityDirection() const;

	/**  */
	UFUNCTION(BlueprintCallable, Category = "Pawn|Components|SixDOFMovementComponent")
		virtual bool IsSpeedBoost() const;

	/**  */
	UFUNCTION(BlueprintCallable, Category = "Pawn|Components|SixDOFMovementComponent")
		virtual bool IsJumping() const;


	/***/
	UFUNCTION(BlueprintCallable, Category = "Pawn|Components|SixDOFMovementComponent")
		FVector GetFallingVelocity() const;

	/***/
	UFUNCTION(BlueprintCallable, Category = "Pawn|Components|SixDOFMovementComponent")
		FVector GetMovementVelocity() const;

	/***/
	UFUNCTION(BlueprintCallable, Category = "Pawn|Components|SixDOFMovementComponent")
		float GetFallingSpeed() const;

	/***/
	UFUNCTION(BlueprintCallable, Category = "Pawn|Components|SixDOFMovementComponent")
		float GetCurrentMovementSpeed() const;

	/***/
	UFUNCTION(BlueprintCallable, Category = "Pawn|Components|SixDOFMovementComponent")
		float GetTotalTimeInAir() const;

protected:

	/**The Updated component*/
	UCapsuleComponent* CapsuleComponent;

	/** Custom movement component owner */
	class ASixDOFPawn* ComponentOwner;

private:

	

	bool bDebugIsEnabled;

	bool bIsInAir;

	bool bIsOnWalkableSurface;

	FQuat CurrentCapsuleQuat;

	float TimeInAir;

	bool bIsSpeedBoost;

	bool bIsJumping;

};
