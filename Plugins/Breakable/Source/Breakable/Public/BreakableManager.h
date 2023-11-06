// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Kismet/KismetMathLibrary.h" 
#include "Kismet/GameplayStatics.h"
//#include "Kismet/KismetArrayLibrary.h"
#include "Materials/MaterialInterface.h" 
#include "Sound/SoundWave.h"
#include "Components/AudioComponent.h"
#include "Components/HierarchicalInstancedStaticMeshComponent.h"
#include "BreakableManager.generated.h"

UCLASS()
class BREAKABLE_API ABreakableManager : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ABreakableManager();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	virtual void OnConstruction(const FTransform& Transform) override;

public:	

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = Setup)
		TObjectPtr<USceneComponent> Root = nullptr;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = Setup)
		TArray<TObjectPtr<UHierarchicalInstancedStaticMeshComponent>> HISM_List;
	
	//Used for finding objects quickly in movement related functions. Set during initialization. Int = HISM_List index, same index for properties lists.
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = Setup)
	TMap<TObjectPtr<UHierarchicalInstancedStaticMeshComponent>, int> HISM_Map;

	TObjectPtr<UHierarchicalInstancedStaticMeshComponent> SelectedHISM = nullptr;

	TObjectPtr<UStaticMesh> ActiveMesh = nullptr;

	//TMap<TObjectPtr<UHierarchicalInstancedStaticMeshComponent>, TArray<int>> HISMBreakable_Map;
	
	//Set HISM_RCD_List and HISM_InitialLocation_List if modifying!
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = Setup)
		int HISM_Count;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = Setup)
	TArray<FVector> HISM_InitialLocation_List;
	
	void SetupHISMs();

	//Row X, Column Y, Depth Z triple list
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = Setup)
		TArray<FIntVector> HISM_RCD_List;
	
	//Row * Column * Depth
	TArray<int> InitRCDMax_List;

	//Sets the max init counter (loop generation) size for each HISM
	void InitializeSettings();

	void InitializeReset();

	FVector SavedMeshScale;

//	TArray<>//
	//if ((SelectedBreakable->isProceduralReplaceable && ProceduralPriority >= SelectedBreakable->ProceduralPriority
	//	&& SelectedBreakable->isBreakable)

	void GenerateTransformList();

	TArray<TArray<FVector>> HISMInstanceTransformList;

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

	TMap<TObjectPtr<UHierarchicalInstancedStaticMeshComponent>, TArray<int>> InstancedPhysicsObjectsMoveMap;

	UFUNCTION(BlueprintCallable, Category = Movement)
		void AddInstancedPhysicsObject(int InstanceIndex, UHierarchicalInstancedStaticMeshComponent* Hit_HISM);

	FTransform InstanceMoveTransform = FTransform(FRotator(0.f, 0.f, 0.f), FVector(0.f, -50000.f, 0.f), FVector(0.f, 0.f, 0.f));

	//Toggle for preview of assets in editor viewport || Warning: Unstable. May crash editor or take significant time to load. Enable and restart editor.
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = Setup)
	bool isPreview = false;

	void InstanceDirectMove();

	FTimerHandle InitializeSurfaceTimerHandle;

	//Max amount of blocks that can be initialized at one time before moving to next frame sequence.
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = Setup)
		int InitMaxBlocks = 20000;
	int InitBlocksCtr = 0;
	int InitBlocksLastIndex = 0;
	int InitHISMLastIndex = 0;


	UFUNCTION(BlueprintCallable, Category = Initialize)
		void ProceduralReplaceBreakables();	

	void AddScore(int HISMIndex);

	//Values should be set with parity to HISM_List
	//Used for Breakables that are intended to replace OTHER direct breakable instances
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = Setup)
		TArray<bool> isProceduralReplaceable_List;
	//Values should be set with parity to HISM_List
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = Setup)
		TArray<bool> isProceduralReplacement_List;	
	//Values should be set with parity to HISM_List
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = Setup)
		TArray<bool> isBreakable_List;
	//Values should be set with parity to HISM_List
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = Setup)
		TArray<FVector> MeshScale_List;
	//Values should be set with parity to HISM_List
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = Setup)
		TArray<int> IncrementSize_List;
	//Values should be set with parity to HISM_List
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = Setup)
		TArray<int> ProceduralReplacementAmountMax_List;
	//Values should be set with parity to HISM_List
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = Setup)
		TArray<int> PointsValue_List;

	TArray<int> ReplaceableHISM_IndexList;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = Setup)
		TArray<TObjectPtr<UMaterialInterface>> InstanceMeshMaterial_List;;
		
	///void InitializeBreakableMap();

	//TODO: Read/Write from file.

	FTimerHandle GameStartedTimerHandle;

	void SetGameStarted();

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = Score)
		int GetScore() { return PointsTotal; };

	UFUNCTION(BlueprintCallable, Category = Initialize)
		void RegenerateProcedurals();

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = Setup)
	float LevelLoadPercent = 0.f;

	TObjectPtr<USoundConcurrency> SoundConcurrency = nullptr;

	TObjectPtr<USoundAttenuation> SoundAttenuation = nullptr;

	TObjectPtr<USoundWave> Sound1 = nullptr;
	TObjectPtr<USoundWave> Sound2 = nullptr;

	void PlaySoundBreakBlock();
	FTransform SoundLocation;
	TObjectPtr<USoundWave> CurrentSound = nullptr;

private:	
	int PointsTotal = 0;
	bool isGameStarted = false;
};
