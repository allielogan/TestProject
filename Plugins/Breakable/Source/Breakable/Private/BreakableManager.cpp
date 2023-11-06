// Fill out your copyright notice in the Description page of Project Settings.


#include "BreakableManager.h"



// Sets default values
ABreakableManager::ABreakableManager()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	Root = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	RootComponent = Root;

	static ConstructorHelpers::FObjectFinder<UStaticMesh> CubeMeshAsset(TEXT("StaticMesh'/Game/StarterContent/Shapes/Shape_Cube.Shape_Cube'"));
	if (CubeMeshAsset.Succeeded())
	{
		ActiveMesh = CubeMeshAsset.Object;
	}
	static ConstructorHelpers::FObjectFinder<USoundWave> SoundAsset1(TEXT("SoundWave'/Game/Custom/SFX/put_magazine_on_table_002.put_magazine_on_table_002'"));
	static ConstructorHelpers::FObjectFinder<USoundWave> SoundAsset2(TEXT("SoundWave'/Game/Custom/SFX/shut_small_book_002.shut_small_book_002'"));

	static ConstructorHelpers::FObjectFinder<USoundConcurrency> SoundC1(TEXT("SoundConcurrency'/Game/Custom/SFX/DefaultSoundConcurrency.DefaultSoundConcurrency'"));
	static ConstructorHelpers::FObjectFinder<USoundAttenuation> SoundA1(TEXT("SoundAttenuation'/Game/Custom/SFX/DefaultSoundAttenuation.DefaultSoundAttenuation'"));

	SoundAttenuation = SoundA1.Object;
	SoundConcurrency = SoundC1.Object;
	Sound1 = SoundAsset1.Object;
	Sound2 = SoundAsset2.Object;

	///Script/Engine.SoundAttenuation'/Game/Custom/SFX/DefaultSoundAttenuation.DefaultSoundAttenuation'
	///Script/Engine.SoundConcurrency'/Game/Custom/SFX/DefaultSoundConcurrency.DefaultSoundConcurrency'
}

void ABreakableManager::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);

	if (isPreview)
	{
		//////InitializeReset();
		//////InitializeSettings();
		//////InitializeSurfaceInstances();
	}
	
	//////GetWorldTimerManager().SetTimer(InitializeSurfaceTimerHandle, this, &ABreakableManager::InitializeSurfaceInstances, DeltaSeconds, false);
}

// Called when the game starts or when spawned
void ABreakableManager::BeginPlay()
{
	Super::BeginPlay();
	SetupHISMs();
	///SetGM();
	SetDeltaSeconds();
	//isPreview = false;
	//UE_LOG(LogTemp, Warning, TEXT("Breakable Cube Begin Play"));
	if (!isPreview)
	{
		InitializeReset();
		InitializeSettings();
		InitializeSurfaceInstances();
	}
	GetWorldTimerManager().SetTimer(InstancePhysicsMoveTimerHandle, this, &ABreakableManager::InstanceDirectMove, DeltaSeconds, true);
}

void ABreakableManager::SetupHISMs()
{
	for (int i = 0; i < HISM_Count; i++)
	{
		SelectedHISM = NewObject<UHierarchicalInstancedStaticMeshComponent>(this);
		SelectedHISM->RegisterComponent();
		SelectedHISM->SetStaticMesh(ActiveMesh);
		SelectedHISM->SetMaterial(0, InstanceMeshMaterial_List[i]);
		SelectedHISM->SetEnableGravity(false);
		SelectedHISM->SetCastShadow(false);
		SelectedHISM->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
		SelectedHISM->SetCollisionObjectType(ECollisionChannel::ECC_WorldDynamic);
		SelectedHISM->SetCollisionResponseToChannel(ECollisionChannel::ECC_Visibility, ECollisionResponse::ECR_Block);
		SelectedHISM->SetCollisionResponseToChannel(ECollisionChannel::ECC_Camera, ECollisionResponse::ECR_Block);
		SelectedHISM->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Block);
		SelectedHISM->SetCollisionResponseToChannel(ECollisionChannel::ECC_WorldStatic, ECollisionResponse::ECR_Ignore);
		SelectedHISM->SetCollisionResponseToChannel(ECollisionChannel::ECC_WorldDynamic, ECollisionResponse::ECR_Ignore);		
		SelectedHISM->SetCollisionResponseToChannel(ECollisionChannel::ECC_PhysicsBody, ECollisionResponse::ECR_Ignore);
		SelectedHISM->SetCollisionResponseToChannel(ECollisionChannel::ECC_Vehicle, ECollisionResponse::ECR_Ignore);
		SelectedHISM->SetCollisionResponseToChannel(ECollisionChannel::ECC_Destructible, ECollisionResponse::ECR_Ignore);
		SelectedHISM->SetMobility(EComponentMobility::Static);
		SelectedHISM->SetComponentTickEnabled(false);
		SelectedHISM->SetAffectDistanceFieldLighting(false);
		SelectedHISM->SetAffectDynamicIndirectLighting(false);
		this->AddInstanceComponent(SelectedHISM);
		HISM_List.Add(SelectedHISM);	
		//TODO: Make sure to turn off shadows, set collision, etc.
	}
}

void ABreakableManager::SetDeltaSeconds()
{
	DeltaSeconds = 0.0333; //30fps
	//DeltaSeconds = 0.0166666666666667; //60fps

}

void ABreakableManager::InitializeSettings()
{
	for (int i = 0; i < HISM_List.Num(); i++)
	{
		//InitCtrMax = Row * Column * Depth;
		auto RCD = HISM_RCD_List[i];
		InitRCDMax_List.Add(RCD.X * RCD.Y * RCD.Z);
		HISM_Map.Add(HISM_List[i], i);
		///TArray<int> NewIntArray;
		///HISMBreakable_Map.Add(HISM_List[i], NewIntArray);

		if (isProceduralReplaceable_List[i])
		{
			ReplaceableHISM_IndexList.Add(i);
		}
	}

	//InitialTransform = GetActorTransform(); //Unneeded if HISM_InitialLocation_List is properly Initialized

	GenerateTransformList();
}

void ABreakableManager::GenerateTransformList()
{
	for (int j = 0; j < HISM_List.Num(); j++)
	{
		int RowSizeCtr = 1;
		int ColSizeCtr = 1;
		int DepSizeCtr = 1;
		LocAdjustX = 0.f;
		LocAdjustY = 0.f;
		LocAdjustZ = 0.f;
		TArray<FVector> NewInstanceTransformList;
		HISMInstanceTransformList.Add(NewInstanceTransformList);

		for (int i = 0; i < InitRCDMax_List[j]; i++)
		{
			//Update & add to list each loop for the adjusted positions representing Cube shape.
			HISMInstanceTransformList[j].Add(HISM_InitialLocation_List[j] + FVector(LocAdjustX, LocAdjustY, LocAdjustZ));
	
			//Row Finished -> Move to next column
			if (RowSizeCtr >= HISM_RCD_List[j].X)
			{
				RowSizeCtr = 1;
				LocAdjustX = 0.f;
				ColSizeCtr++;
				//Column Finished -> Move to next Depth
				if (ColSizeCtr > HISM_RCD_List[j].Y)
				{
					ColSizeCtr = 1;
					LocAdjustY = 0.f;
					DepSizeCtr++;
					//Depth finished -> Loop should be completed here.
					if (DepSizeCtr > HISM_RCD_List[j].Z)
					{
						LocAdjustZ = 0.f;
					}
					//Update Depth Location to next Z Depth by increment.
					else
					{
						LocAdjustZ += IncrementSize_List[j];
					}
				}
				//Update Column Location to next Y Column by increment.
				else
				{
					LocAdjustY += IncrementSize_List[j];
				}
			}
			//Update Row Location to next X Row by increment.
			else
			{
				LocAdjustX += IncrementSize_List[j];
				RowSizeCtr++;
			}
		}
	}
	LevelLoadPercent = 20.f;
}

void ABreakableManager::InitializeSurfaceInstances()
{
	//	GetWorldTimerManager().SetTimer(InitializeSurfaceTimerHandle, this, &ABreakableManager::InitializeSurfaceInstances, DeltaSeconds, false);
	//int InitMaxBlocks = 100;
	//int InitBlocksCtr = 0;
	if (useMaxSetting)
	{//InitHISMLastIndex
		for (int j = InitHISMLastIndex; j < HISM_List.Num(); j++)
		{
			
			if (j != InitHISMLastIndex)
			{
				InitHISMLastIndex = j;
				InitBlocksLastIndex = 0;
			}
			for (int i = InitBlocksLastIndex; i < InitRCDMax_List[j]; i++)
			{
				InitBlocksCtr++;
				if (InitBlocksCtr >= InitMaxBlocks)
				{
					InitBlocksCtr = 0;
					InitHISMLastIndex = j;
					InitBlocksLastIndex = i;
					GetWorldTimerManager().SetTimer(InitializeSurfaceTimerHandle, this, &ABreakableManager::InitializeSurfaceInstances, DeltaSeconds, false);
					return;
				}
				if (HISM_List[j] && HISMInstanceTransformList[j].Num() > 0)
				{
					//Rotation set to 0. Do Not Modify. 
					HISM_List[j]->AddInstance(FTransform(FRotator(0.f, 0.f, 0.f), HISMInstanceTransformList[j][i], MeshScale_List[j]), true);
				}
			}
			if (j >= 0 && j <= 5)
			{
				LevelLoadPercent += 10.f;
			}
		}
	}
	else
	{
		for (int j = InitHISMLastIndex; j < HISM_List.Num(); j++)
		{
			for (int i = 0; i < InitRCDMax_List[j]; i++)
			{
				if (HISM_List[j] && HISMInstanceTransformList[j].Num() > 0)
				{
					HISM_List[j]->AddInstance(FTransform(FRotator(0.f, 0.f, 0.f), HISMInstanceTransformList[j][i], MeshScale_List[j]), true);
				}
			}
			if (j >= 0 && j <= 5)
			{
				LevelLoadPercent += 10.f;
			}
		}
	}
	//InitializeBreakableMap();
	ProceduralReplaceBreakables();
}

//TODO: Check Trace accounts for HISM in List. Make sure HISM is 	if (isBreakable == true)
//TODO: Should have Trace send a batch Array of points so can cycle instead of calling and constructing objects each time.
void ABreakableManager::AddInstancedPhysicsObject(int InstanceIndex, UHierarchicalInstancedStaticMeshComponent* Hit_HISM)
{
	int* HISMIndex = HISM_Map.Find(Hit_HISM);
	if (HISMIndex != nullptr)
	{
		if (isBreakable_List[*HISMIndex])
		{
			TArray<int>* HISM_FoundIntArray = InstancedPhysicsObjectsMoveMap.Find(Hit_HISM);
			if (HISM_FoundIntArray != nullptr)
			{
				HISM_FoundIntArray->Add(InstanceIndex);
			}
			else
			{
				TArray<int> NewIntArray{ InstanceIndex };
				InstancedPhysicsObjectsMoveMap.Add(Hit_HISM, NewIntArray);
			}
		}
	}
	else { return; }	
}

void ABreakableManager::InitializeReset()
{
	HISMInstanceTransformList.Empty();
	for (auto H : HISM_List)
	{
		H->ClearInstances();
	}	
}

void ABreakableManager::InstanceDirectMove()
{
	bool UpdateRequired = false;
	TArray<int> InstancedPhysicsObjectsMoveMap_KeyList;
	int LastInstanceIndex = 0;
	TArray<int>* HISM_MoveList = nullptr;
	TObjectPtr<UHierarchicalInstancedStaticMeshComponent> LastHISM = nullptr;

	//for each HISM in the map generate value array of all hit indices.
	//TPair<TObjectPtr<UHierarchicalInstancedStaticMeshComponent>, TArray<int>>
	for (auto H : InstancedPhysicsObjectsMoveMap)
	{		
		HISM_MoveList = &H.Value;
		//for each int/hit index in instance list update transform offscreen & remove from move map.
		for (int i : *HISM_MoveList)
		{
			H.Key->GetInstanceTransform(i, SoundLocation, true);
			
			UpdateRequired = true;
			H.Key->UpdateInstanceTransform(i, InstanceMoveTransform, true, false);
			LastInstanceIndex = i;
			//Items are not removed from instancing due to negative effects on restructuring the Instance List -> Instead they are moved far off screen where they will not affect running threads.		
			//InstancedPhysicsObjectsMoveMap.Remove(i);
			//int HISM_Index = 			
			int HISM_Index = HISM_Map[H.Key];
			if (isGameStarted) 
			{
				AddScore(HISM_Index);
				PlaySoundBreakBlock();
			}			
		}
		LastHISM = H.Key;
		//No need to remove indices individually, can just remove the HISM when finished.
		InstancedPhysicsObjectsMoveMap.Remove(H.Key);
	}
	if (UpdateRequired)
	{
		//Updating any instance as bDirty true updates ALL instances.
		LastHISM->UpdateInstanceTransform(LastInstanceIndex, InstanceMoveTransform, true, true);
	}
}

void ABreakableManager::ProceduralReplaceBreakables()
{
	//UE_LOG(LogTemp, Warning, TEXT("Replacement Blocks Loading..."));
	int RandomBreakableInt = 0;
	int RandomBreakableIndex = 0; //Unused except for OutArray
	int RandomInstanceInt = 0;
	int RandomInstanceIndex = 0; //Unused except for OutArray
	FTransform InstanceTransform;
	//Loop through HISMs
	for (int j = 0; j < HISM_List.Num(); j++)
	{
		if (!isProceduralReplacement_List[j])
		{
			j++;
		}
		else
		{
			//Loop for how many instances to replace.
			for (int i = 0; i < ProceduralReplacementAmountMax_List[j]; i++)
			{
				//UKismetArrayLibrary::Array_Random(ReplaceableHISM_IndexList, RandomBreakableInt, RandomBreakableIndex);
				RandomBreakableInt = FMath::RandRange(1, 5);

				if (isProceduralReplaceable_List[RandomBreakableInt] == true)
				{
					//Select a Random Breakable Instance to replace
					RandomInstanceInt = FMath::RandRange(0, HISM_List[RandomBreakableInt]->GetInstanceCount() - 1);
					HISM_List[RandomBreakableInt]->GetInstanceTransform(RandomInstanceInt, InstanceTransform, true);
					//Create Instance on Self to replace Transform of selected breakable.
					HISM_List[j]->AddInstance(InstanceTransform, true);
					//Adding to this map will auto destroy the instance for the Selected Breakable.
					AddInstancedPhysicsObject(RandomInstanceInt, HISM_List[RandomBreakableInt]);
				}
				else { i--; }
				//Breakable is higher priority or not replaceable, remove from list and decrement loop.			
			}		
		}
		if (j >= 6 && j <= 11)
		{
			LevelLoadPercent += 3.f;
		}
	}	
	//UE_LOG(LogTemp, Warning, TEXT("Replacement Blocks Finished Loading."));
	GetWorldTimerManager().SetTimer(GameStartedTimerHandle, this, &ABreakableManager::SetGameStarted, 2.f, false);
	
}

void ABreakableManager::AddScore(int HISMIndex)
{	
	PointsTotal += PointsValue_List[HISMIndex];
}

void ABreakableManager::SetGameStarted()
{
	isGameStarted = true;
	LevelLoadPercent = 100.f;
}

/*
void ABreakableManager::InitializeBreakableMap()
{
	//HISMBreakable_Map.Add(HISM_List[i], NewIntArray);
	int j = 0;
	TObjectPtr<TArray<int>> BreakableKeys;
	for (auto H : HISM_List)
	{
		if (isBreakable_List[j])
		{
			int HISM_Size = H->GetInstanceCount();
			///BreakableKeys = &HISMBreakable_Map[H];
			for (int i = 0; i < HISM_Size; i++)
			{				
				BreakableKeys->Add(i);
			}			
		}
		j++;
	}	
	ProceduralReplaceBreakables();
}
*/

void ABreakableManager::RegenerateProcedurals()
{
	isGameStarted = false;
	for (int j = 0; j < HISM_List.Num(); j++)
	{
		if (j >= 6 && j <= 11)
		{
			HISM_List[j]->ClearInstances();
		}
		else
		{
			for (int i = 0; i < InitRCDMax_List[j]; i++)
			{
				if (HISM_List[j] && HISMInstanceTransformList[j].Num() > 0)
				{
					HISM_List[j]->UpdateInstanceTransform(i, FTransform(FRotator(0.f, 0.f, 0.f), HISMInstanceTransformList[j][i], MeshScale_List[j]), true, false);
				}
			}
		}		
	}
	HISM_List[0]->UpdateInstanceTransform(0, FTransform(FRotator(0.f, 0.f, 0.f), HISMInstanceTransformList[0][0], MeshScale_List[0]), true, true);

	//InitializeBreakableMap();
	ProceduralReplaceBreakables();	
}

void ABreakableManager::PlaySoundBreakBlock()
{
	int RandomSoundInt = FMath::RandRange(1, 2);
	CurrentSound;
	switch (RandomSoundInt)
	{
	default: case 1:
		CurrentSound = Sound1;
		break;
	case 2:
		CurrentSound = Sound2;
		break;
	}	

	UGameplayStatics::SpawnSoundAtLocation(this, CurrentSound, SoundLocation.GetLocation(), FRotator(0.f, 0.f, 0.f),
		1.f, 1.f, 0.f, SoundAttenuation, SoundConcurrency, true);
}