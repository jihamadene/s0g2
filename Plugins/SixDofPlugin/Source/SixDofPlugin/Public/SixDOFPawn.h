// Copyright 1998-2015 Epic Games, Inc. All Rights Reserved.


#pragma once

#include "SixDOFMovementComponent.h"
#include "SixDOFPawn.generated.h"



UCLASS()
class  SIXDOFPLUGIN_API ASixDOFPawn : public APawn
{
	GENERATED_BODY()

public:



	UCharacterMovementComponent* a;
	/**
	* Default UObject constructor.
	*/
	ASixDOFPawn(const FObjectInitializer& ObjectInitializer);

	// APawn interface
	virtual void PostInitializeComponents() override;
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaSeconds) override;
	virtual void NotifyHit(class UPrimitiveComponent* MyComp, class AActor* Other, class UPrimitiveComponent* OtherComp, bool bSelfMoved, FVector HitLocation, FVector HitNormal, FVector NormalImpulse, const FHitResult& Hit) override;
	// End of AActor interface


	/**  */
	UFUNCTION(BlueprintCallable, Category = "Pawn|SixDOFPawn|Input", meta = (Keywords = "AddInput"))
		void AddForwardMovementInput(float ScaleValue = 1.0f, bool bForce = false);

	/**  */
	UFUNCTION(BlueprintCallable, Category = "Pawn|SixDOFPawn|Input", meta = (Keywords = "AddInput"))
		void AddRightMovementInput(float ScaleValue = 1.0f, bool bForce = false);

	/**  */
	UFUNCTION(BlueprintCallable, Category = "Pawn|SixDOFPawn|Input", meta = (Keywords = "AddInput"))
		void AddUpMovementInput(float ScaleValue = 1.0f, bool bForce = false);
	
	/**	*/
	UFUNCTION(BlueprintCallable, Category = "Pawn|SixDOFPawn|Input", meta = (Keywords = "AddInput"))
		void AddPawnPitchInput(float UpdateRate = 1.0f, float ScaleValue = 0.0f, bool bEnableCameraPitchControlOnWalk=true);

	/***/
	UFUNCTION(BlueprintCallable, Category = "Pawn|SixDOFPawn|Input", meta = (Keywords = "AddInput"))
		void AddPawnYawInput(float UpdateRate = 1.0f, float ScaleValue = 0.0f);

	/***/
	UFUNCTION(BlueprintCallable, Category = "Pawn|SixDOFPawn|Input", meta = (Keywords = "AddInput"))
		void AddPawnRollInput(float UpdateRate = 1.0f, float ScaleValue = 0.0f);

	/** */
	UFUNCTION(BlueprintCallable, Category = "Pawn|SixDOFPawn|Input")
		void PawnLaunch(FVector LaunchVelocity, bool bOverrideCurrentVelocity);

	/***/
	UFUNCTION(BlueprintCallable, Category = "Pawn|SixDOFPawn|Input")
		void PawnLaunchToTarget(class AActor* Target, float LaunchPower, bool bOverrideCurrentVelocity);

	/**  */
	UFUNCTION(BlueprintCallable, Category = "Pawn|SixDOFPawn|Input", meta = (Keywords = "AddInput"))
		void Jump();

	/**  */
	UFUNCTION(BlueprintCallable, Category = "Pawn|SixDOFPawn|Input", meta = (Keywords = "AddInput"))
		void SpeedBoost();

	/** */
	UFUNCTION(BlueprintCallable, Category = "Pawn|SixDOFPawn|Input", meta = (Keywords = "AddInput"))
		void StopSpeedBoost();


	/** */
	UFUNCTION(BlueprintCallable, Category = "Pawn|SixDOFPawn|Debuging")
		void EnableDebugging();

	/**  */
	UFUNCTION(BlueprintCallable, Category = "Pawn|SixDOFPawn|Debuging")
		void DisableDebugging();

private:

	/** The CapsuleComponent being used for movement collision (by CharacterMovement).*/
	UPROPERTY(Category = "Six DOF Pawn", VisibleDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
		UCapsuleComponent* CapsuleComponent;

	/** The camera boom. */
	UPROPERTY(Category = "Six DOF Pawn", VisibleDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
		USpringArmComponent* SpringArm;

	/** the main camera associated with this Pawn . */
	UPROPERTY(Category = "Six DOF Pawn", VisibleDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
		UCameraComponent* Camera;

	/** Movement component used for movement. */
	UPROPERTY(Category = "Six DOF Pawn", VisibleDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
		USixDOFMovementComponent* MovementComponent;

	/** Skeletal mesh associated with this Pawn. */
	UPROPERTY(Category = "Six DOF Pawn", VisibleDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
		USkeletalMeshComponent* PawnMesh;

	/** Gizmo used as debug arrows root component. */
	UPROPERTY(Category = "Six DOF Pawn", VisibleDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
		USceneComponent* GizmoRootComponent;

	/**Forward Arrow Component*/
	UPROPERTY(Category = "Six DOF Pawn", VisibleDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
		UArrowComponent* ForwardArrowComponent;

	/**Right Arrow Component*/
	UPROPERTY(Category = "Six DOF Pawn", VisibleDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
		UArrowComponent* RightArrowComponent;

	/**Up Arrow Component. */
	UPROPERTY(Category = "Six DOF Pawn", VisibleDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
		UArrowComponent* UpArrowComponent;

public:

	/** Returns CapsuleComponent subobject **/
	FORCEINLINE class UCapsuleComponent* GetCapsuleComponent() const{ return CapsuleComponent; }

	/** Returns PawnMesh subobject **/
	FORCEINLINE class USkeletalMeshComponent* GetMesh() const { return PawnMesh; }

	/** Returns SpringArm subobject **/
	FORCEINLINE class USpringArmComponent* GetSpringArm() const { return SpringArm; }

	/** Returns Camera subobject **/
	FORCEINLINE class UCameraComponent* GetCamera() const { return Camera; }

	/** Returns CustomMovement Component subobject **/
	FORCEINLINE class USixDOFMovementComponent* GetMovementComponent() const { return  MovementComponent; }

	/** Returns Gizmo SceneComponent subobject **/
	FORCEINLINE class USceneComponent* GetGizmoRootComponent() const{ return  GizmoRootComponent; }

	/** Returns Forward ArrowComponent subobject **/
	FORCEINLINE class UArrowComponent* GetForwardArrowComponent() const{ return ForwardArrowComponent; }

	/** Returns Right ArrowComponent subobject **/
	FORCEINLINE class UArrowComponent* GetRightArrowComponent() const{ return RightArrowComponent; }

	/** Returns Up ArrowComponent subobject **/
	FORCEINLINE class UArrowComponent* GetUpArrowComponent() const{ return  UpArrowComponent; }


};