// Fill out your copyright notice in the Description page of Project Settings.


#include "BreakableCube.h"

// Sets default values
ABreakableCube::ABreakableCube()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;	

	Root = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	RootComponent = Root;
	//ISM = CreateDefaultSubobject<UInstancedStaticMeshComponent>(TEXT("Instanced Static Mesh Component"));
	ISM = CreateDefaultSubobject<UHierarchicalInstancedStaticMeshComponent>(TEXT("H Instanced Static Mesh Component"));

	static ConstructorHelpers::FObjectFinder<UStaticMesh> CubeMeshAsset(TEXT("StaticMesh'/Game/StarterContent/Shapes/Shape_Cube.Shape_Cube'"));
	if (CubeMeshAsset.Succeeded())
	{
		ISM->SetStaticMesh(CubeMeshAsset.Object);
	}

	ISM->SetupAttachment(Root);	
}

void ABreakableCube::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);

	//InitializeReset();
	//InitializeSettings(RowSize, ColumnSize, DepthSize, IncrementSize);
	//InitializeSurfaceInstances();
	GetWorldTimerManager().SetTimer(InitializeSurfaceTimerHandle, this, &ABreakableCube::InitializeSurfaceInstances, DeltaSeconds, false);
}

// Called when the game starts or when spawned
void ABreakableCube::BeginPlay()
{
	Super::BeginPlay();
	SetGM();
	SetDeltaSeconds();
	isPreview = false;
	//UE_LOG(LogTemp, Warning, TEXT("Breakable Cube Begin Play"));
	if (!isProceduralReplacement)
	{
		InitializeReset();
		InitializeSettings(RowSize, ColumnSize, DepthSize, IncrementSize);
		InitializeSurfaceInstances();
	}
	else
	{
		//GetWorldTimerManager().SetTimer(CheckBlocksLoadedTimerHandle, this, &ABreakableCube::TraceCheckBlocksLoaded, 5.f, false);
		
	}
	/*GetWorldTimerManager().SetTimer(InstancePhysicsMoveTimerHandle, this, &ABreakableCube::InstancePhysicsObjectsMove, DeltaSeconds, true);*/
	if (isBreakable == true) 
	{
		GetWorldTimerManager().SetTimer(InstancePhysicsMoveTimerHandle, this, &ABreakableCube::InstanceDirectMove, DeltaSeconds, true);
	}	
	///InstancePhysicsMoveDelegate.BindUFunction(this, "InstancePhysicsObjectsMove", FVector(1040.f, 1260.f, 500.f));
	///GetWorldTimerManager().SetTimer(InstancePhysicsMoveTimerHandle, InstancePhysicsMoveDelegate, DeltaSeconds, true);
	/*
	if (HitBreakableCube->isFinishedLoading)
	GetWorldTimerManager().SetTimer(CheckBlocksLoadedTimerHandle, this, &ABreakableCube::TraceCheckBlocksLoaded, 2.f, false);
	*/
}

void ABreakableCube::SetDeltaSeconds()
{	
	DeltaSeconds = 0.0333;
	//DeltaSeconds = 0.0166666666666667;

}

void ABreakableCube::SetGM()
{
	GM = Cast<ATestProjectGameMode>(GetWorld()->GetAuthGameMode());
}

void ABreakableCube::InitializeSettings(int Row, int Column, int Depth, float Increment)
{
	
	InitialTransform = GetActorTransform();

	//RowSize = Row; ColumnSize = Column; DepthSize = Depth; IncrementSize = Increment;

	if (isPreview)
	{		
		/*
		MeshScale = SavedMeshScale;
		float X = Row * MeshScale.X;
		float Y = Column * MeshScale.Y;
		float Z = Depth * MeshScale.Z;
		InitCtrMax = 1;
		SavedMeshScale = MeshScale;
		MeshScale = FVector(X, Y, Z);
		*/
	}
	else
	{
		//InitCtrMax = Row * Column * Depth;
	}	
	InitCtrMax = Row * Column * Depth;
	GenerateTransformList();
}

void ABreakableCube::GenerateTransformList()
{
	int RowSizeCtr = 1;
	int ColSizeCtr = 1;
	int DepSizeCtr = 1;

	for (int i = 0; i < InitCtrMax; i++)
	{
		//Update & add to list each loop for the adjusted positions representing Cube shape.
		InitializerTransformList.Add(InitialTransform.GetLocation() + FVector(LocAdjustX, LocAdjustY, LocAdjustZ));

		//Row Finished -> Move to next column
		if (RowSizeCtr >= RowSize)
		{
			RowSizeCtr = 1;
			LocAdjustX = 0.f;
			ColSizeCtr++;
			//Column Finished -> Move to next Depth
			if (ColSizeCtr > ColumnSize)
			{
				ColSizeCtr = 1;
				LocAdjustY = 0.f;
				DepSizeCtr++;
				//Depth finished -> Loop should be completed here.
				if (DepSizeCtr > DepthSize)
				{
					LocAdjustZ = 0.f;
				}
				//Update Depth Location to next Z Depth by increment.
				else
				{
					LocAdjustZ += IncrementSize;
				}
			}
			//Update Column Location to next Y Column by increment.
			else
			{
				LocAdjustY += IncrementSize;
			}
		}
		//Update Row Location to next X Row by increment.
		else
		{
			LocAdjustX += IncrementSize;
			RowSizeCtr++;
		}

	}	
}

void ABreakableCube::InitializeSurfaceInstances()
{	
	//	GetWorldTimerManager().SetTimer(InitializeSurfaceTimerHandle, this, &ABreakableCube::InitializeSurfaceInstances, DeltaSeconds, false);
	//int InitMaxBlocks = 100;
	//int InitBlocksCtr = 0;
	if (useMaxSetting)
	{
		InitBlocksCtr = 0;
		for (int i = InitBlocksLastIndex; i < InitCtrMax; i++)
		{
			InitBlocksCtr++;
			if (InitBlocksCtr >= InitMaxBlocks)
			{
				InitBlocksLastIndex = i;
				GetWorldTimerManager().SetTimer(InitializeSurfaceTimerHandle, this, &ABreakableCube::InitializeSurfaceInstances, DeltaSeconds, false);
				break;
			}
			if (ISM && InitializerTransformList.Num() > 0)
			{
				ISM->AddInstance(FTransform(InitialTransform.GetRotation(), InitializerTransformList[i], MeshScale), true);
			}
		}
	}
	else
	{
		for (int i = 0; i < InitializerTransformList.Num(); i++)
		{
			if (ISM && InitializerTransformList.Num() > 0)
			{
				ISM->AddInstance(FTransform(InitialTransform.GetRotation(), InitializerTransformList[i], MeshScale), true);
			}
		}		
	}
	isFinishedLoading = true;
}


void ABreakableCube::InstancePhysicsObjectsMove()
{
	//UE_LOG(LogTemp, Warning, TEXT("Instance Physics Objects Move Begin."));
	TArray<int> InstancedPhysicsObjectsMoveMap_KeyList;
	int LastInstanceIndex = 0;

	InstancedPhysicsObjectsMoveMap.GenerateKeyArray(InstancedPhysicsObjectsMoveMap_KeyList);

	//if ISM is not set return out of function.
	if (!ISM) { return; }
	FTransform InstanceTransform;
	for (int i : InstancedPhysicsObjectsMoveMap_KeyList)
	{			
		
		//UE_LOG(LogTemp, Warning, TEXT("Updating Instance Location..."));
		ISM->GetInstanceTransform(i, InstanceTransform, false);		
		
		CurrentLocation = ISM->K2_GetComponentLocation() + InstanceTransform.GetLocation();

		ISM->UpdateInstanceTransform(i, FTransform(
			UKismetMathLibrary::ComposeRotators(InstanceTransform.Rotator(), MovePhysicsRotationSpeed),
			UKismetMathLibrary::VInterpTo_Constant(CurrentLocation, GetInstancePhysicsMoveToLocation(), DeltaSeconds, InstancedPhysicsObjectMoveSpeed),
			InstanceTransform.GetScale3D()
			), true, false);
		
		LastInstanceIndex = i;
		//Items are not removed from instancing due to negative effects on restructuring the Instance List -> Instead they are moved far off screen where they will not affect running threads.
		if (UKismetMathLibrary::EqualEqual_VectorVector(CurrentLocation, GetInstancePhysicsMoveToLocation(), 1.f))
		{
			ISM->UpdateInstanceTransform(LastInstanceIndex, FTransform(
				FRotator(0.f, 0.f, 0.f), 
				FVector(0.f, -50000.f, 0.f), 
				InstanceTransform.GetScale3D()), true, true);
			//On next Key generation this object will not appear.
			InstancedPhysicsObjectsMoveMap.Remove(i);
		}
		if (UKismetMathLibrary::Vector_Distance(CurrentLocation, GetInstancePhysicsMoveToLocation()) > 1000.f)
		{
			InstancedPhysicsObjectsMoveMap.Remove(i);
		}
		
	}
	if (InstancedPhysicsObjectsMoveMap_KeyList.Num() > 0)
	{
		//Updates visuals after changes. Dirty = true.	
		ISM->UpdateInstanceTransform(LastInstanceIndex, InstanceTransform, true, true);
	}	
}

void ABreakableCube::AddInstancedPhysicsObject(int InstanceIndex)
{
	InstancedPhysicsObjectsMoveMap.Add(InstanceIndex, 0);
}
//Temporarily Set to static location. TODO: Update based on player cast location.
FVector ABreakableCube::GetInstancePhysicsMoveToLocation()
{
	if (PhysicsMoveToLocation == FVector(0.f, 0.f, 0.f)) { return FVector(1040.f, 1260.f, 500.f); }	
	else { return PhysicsMoveToLocation; }
}

void ABreakableCube::InitializeReset()
{
	InitializerTransformList.Empty();
	ISM->ClearInstances();
}

void ABreakableCube::InstanceDirectMove()
{
	TArray<int> InstancedPhysicsObjectsMoveMap_KeyList;
	int LastInstanceIndex = 0;

	InstancedPhysicsObjectsMoveMap.GenerateKeyArray(InstancedPhysicsObjectsMoveMap_KeyList);

	FTransform InstanceTransform = FTransform(FRotator(0.f, 0.f, 0.f), FVector(0.f, -50000.f, 0.f), FVector(0.f, 0.f, 0.f));
	//if ISM is not set return out of function.
	if (!ISM) { return; }	
	for (int i : InstancedPhysicsObjectsMoveMap_KeyList)
	{	
		ISM->UpdateInstanceTransform(i, InstanceTransform, true, false);		
		LastInstanceIndex = i;
		//Items are not removed from instancing due to negative effects on restructuring the Instance List -> Instead they are moved far off screen where they will not affect running threads.		
		InstancedPhysicsObjectsMoveMap.Remove(i);
		SetGMScore();
	}
	if (InstancedPhysicsObjectsMoveMap_KeyList.Num() > 0)
	{
		//Updates visuals once after changes. Dirty = true.	
		ISM->UpdateInstanceTransform(LastInstanceIndex, InstanceTransform, true, true);
	}
}

void ABreakableCube::ReplaceBreakablesWithSelf()
{	
	UE_LOG(LogTemp, Warning, TEXT("Replacement Blocks Loading..."));
	//auto Breakable : LevelLandscapeBreakableCubesList
	int RandomBreakableInt = 0;
	int RandomInstanceInt = 0;
	FTransform InstanceTransform;
	TObjectPtr<ABreakableCube> SelectedBreakable = nullptr;
	//Loop for how many instances to replace.
	for (int i = 0; i < ProceduralReplacementAmountMax; i++)
	{		
		if (LevelLandscapeBreakableCubesList.Num() > 0)
		{
			//Get a random Breakable in list
			RandomBreakableInt = FMath::RandRange(0, LevelLandscapeBreakableCubesList.Num() - 1);
			SelectedBreakable = LevelLandscapeBreakableCubesList[RandomBreakableInt];
			//If Breakable is acceptable, replace it
			if ((SelectedBreakable->isProceduralReplaceable && ProceduralPriority >= SelectedBreakable->ProceduralPriority
				&& SelectedBreakable->isBreakable)
				|| (SelectedBreakable->isProceduralReplacement && ProceduralPriority >= SelectedBreakable->ProceduralPriority
					&& SelectedBreakable->isBreakable))
			{
				//Select a Random Breakable Instance to replace
				RandomInstanceInt = FMath::RandRange(0, SelectedBreakable->ISM->GetInstanceCount() - 1);
				SelectedBreakable->ISM->GetInstanceTransform(RandomInstanceInt, InstanceTransform, true);				
				//InstanceTransform.SetLocation(ISM->K2_GetComponentLocation() + InstanceTransform.GetLocation());
				//Create Instance on Self to replace Transform of selected breakable.
				ISM->AddInstance(InstanceTransform, true);
				//Adding to this map will auto destroy the instance for the Selected Breakable.
				SelectedBreakable->AddInstancedPhysicsObject(RandomInstanceInt);
				//SelectedBreakable->InstancedPhysicsObjectsMoveMap.Add(RandomInstanceInt, 0); //Replaced with AddInstancedPhysicsObject
			}
			//Breakable is higher priority or not replaceable, remove from list and decrement loop.
			else
			{
				i--;
				LevelLandscapeBreakableCubesList.Remove(SelectedBreakable);
			}
		}		
	}
	UE_LOG(LogTemp, Warning, TEXT("Replacement Blocks Finished Loading."));
}

void ABreakableCube::TraceCheckBlocksLoaded()
{
	TArray<FHitResult> HitResults;
	
	FVector Start = FVector(1000.f, -1500.f, -1000.f);
	FVector End = FVector(1000.f, -1500.f, 2000.f);

	FCollisionQueryParams QueryParams;
	QueryParams.AddIgnoredActor(this);
	QueryParams.bTraceComplex = true;	
	
	UE_LOG(LogTemp, Warning, TEXT("Trace Checking Blocks Loaded..."));
	GetWorld()->LineTraceMultiByChannel(HitResults, Start, End, ECollisionChannel::ECC_Visibility, QueryParams);

	if (HitResults.Num() > 0)
	{
		for (auto Hit : HitResults)
		{
			if (UKismetMathLibrary::ClassIsChildOf(Hit.GetActor()->GetClass(), ABreakableCube::StaticClass()))
			{
				TObjectPtr<ABreakableCube> HitBreakableCube = Cast<ABreakableCube>(Hit.GetActor());
				if (HitBreakableCube)
				{					
					UE_LOG(LogTemp, Warning, TEXT("Trace adding Breakable: %s"), *HitBreakableCube->GetName());
					LevelLandscapeBreakableCubesList.AddUnique(HitBreakableCube);
				}								
			}
		}
		ReplaceBreakablesWithSelf();
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Trace Checking Blocks Loaded: No Trace Results Found"));
	}
	//GetWorldTimerManager().SetTimer(CheckBlocksLoadedTimerHandle, this, &ABreakableCube::TraceCheckBlocksLoaded, 5.f, false);
}

bool ABreakableCube::GetLoadValue()
{
	return isFinishedLoading;
}

void ABreakableCube::SetGMScore()
{
	if (GM && GM->isGameStarted)
	{
		GM->AddScore(PointsValue);
	}
}
