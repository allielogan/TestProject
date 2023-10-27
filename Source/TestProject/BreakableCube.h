// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Kismet/KismetMathLibrary.h" 
#include "Components/InstancedStaticMeshComponent.h"
#include "Components/HierarchicalInstancedStaticMeshComponent.h"
#include "TestProjectGameMode.h"
#include "BreakableCube.generated.h"

class ATestProjectGameMode;

UCLASS()
class TESTPROJECT_API ABreakableCube : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ABreakableCube();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	virtual void OnConstruction(const FTransform& Transform) override;

public:	
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = Setup)
	TObjectPtr<USceneComponent> Root = nullptr;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = Setup)
	TObjectPtr<UHierarchicalInstancedStaticMeshComponent> ISM = nullptr;

	//Default Row, Column, and Depth sizes to 1, Increment set to 10 for smooth surface.
	void InitializeSettings(int Row = 1, int Column = 1, int Depth = 1, float Increment = 10);	

	void InitializeReset();

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = Setup)
	FTransform InitialTransform;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = Setup)
	int RowSize = 1;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = Setup)
	int ColumnSize = 1;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = Setup)
	int DepthSize = 1;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = Setup)
	float IncrementSize = 10;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = Setup)
	FVector MeshScale = FVector(0.1, 0.1, 0.1);

	FVector SavedMeshScale;

	int InitCtrMax = 0;

	void GenerateTransformList();

	TArray<FVector> InitializerTransformList;

	float LocAdjustX = 0.f;
	float LocAdjustY = 0.f;
	float LocAdjustZ = 0.f;
	
	FTimerHandle InstancePhysicsMoveTimerHandle;
	FTimerDelegate InstancePhysicsMoveDelegate;
	float DeltaSeconds = 0.f;

	void SetDeltaSeconds();

	void InitializeSurfaceInstances();
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = Setup)
	bool useMaxSetting = true;

	UFUNCTION()
	void InstancePhysicsObjectsMove();

	TMap<int, int> InstancedPhysicsObjectsMoveMap;

	UFUNCTION(BlueprintCallable, Category = Movement)
	void AddInstancedPhysicsObject(int InstanceIndex);

	FVector GetInstancePhysicsMoveToLocation();
	FVector PhysicsMoveToLocation = FVector(0.f, 0.f, 0.f);

	FRotator MovePhysicsRotationSpeed = FRotator(5.f, 5.f, 5.f);


	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = Setup)
	float InstancedPhysicsObjectMoveSpeed = 200.f;

	FVector CurrentLocation;

	bool isPreview = true;

	void InstanceDirectMove();

	FTimerHandle InitializeSurfaceTimerHandle;

	//Max amount of blocks that can be initialized at one time before moving to next frame sequence.
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = Setup)
	int InitMaxBlocks = 20000;
	int InitBlocksCtr = 0;
	int InitBlocksLastIndex = 0;

	TObjectPtr<ABreakableCube> HitCube;
	UFUNCTION(BlueprintCallable, Category = Initialize)
	void ReplaceBreakablesWithSelf();
	//Used for Breakables that are intended to replace OTHER direct breakable instances
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = Setup)
	bool isProceduralReplacement = false;

	//Used for Breakables that are NOT intended to be replaced by OTHER procedural breakables -> Grass, etc.
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = Setup)
	bool isProceduralReplaceable = false;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = Setup)
	int ProceduralReplacementAmountMax = 0;

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = Initialize)
	bool GetLoadValue();

	void TraceCheckBlocksLoaded();
	FTimerHandle CheckBlocksLoadedTimerHandle;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = Setup)
	TArray<TObjectPtr<ABreakableCube>> LevelLandscapeBreakableCubesList;

	//If priority is higher -> lower priority cannot replace.
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = Setup)
	int ProceduralPriority = 0;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = Setup)
	bool isBreakable = true;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = Setup)
	int PointsValue = 0;

	TObjectPtr<ATestProjectGameMode> GM = nullptr;

	void SetGM();

	void SetGMScore();
private:
	bool isFinishedLoading = false;
};
