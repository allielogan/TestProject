// Fill out your copyright notice in the Description page of Project Settings.


#include "BreakableAttachment.h"

// Sets default values for this component's properties
UBreakableAttachment::UBreakableAttachment()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = false;

	// ...
}


// Called when the game starts
void UBreakableAttachment::BeginPlay()
{
	Super::BeginPlay();

	// ...
	
}

void UBreakableAttachment::TraceForBreakableInRange()
{	
	if (AttachedCharacter == nullptr || AttachedActor == nullptr) { return; }
	TArray<FHitResult> HitResults;
	TArray<AActor*> IgnoredActors; //empty		

	FVector Start = AttachedActor->GetActorLocation() + (AttachedActor->GetActorForwardVector() * 100.f);
	FVector End = AttachedActor->GetActorLocation() + (AttachedActor->GetActorForwardVector() * AffectedRadius);

	//UKismetSystemLibrary::SphereTraceMulti(GetWorld(), Start, End, AffectedRadius, UEngineTypes::ConvertToTraceType(ECC_Visibility), true,
	//	IgnoredActors, EDrawDebugTrace::ForDuration, HitResults, true, FLinearColor::Red, FLinearColor::Green, 1.f);
	UKismetSystemLibrary::SphereTraceMulti(GetWorld(), Start, End, AffectedRadius, UEngineTypes::ConvertToTraceType(ECC_Visibility), true,
		IgnoredActors, EDrawDebugTrace::None, HitResults, true);
	
	if (HitResults.Num() > 0)
	{
		for (auto Hit : HitResults)
		{
			if (UKismetMathLibrary::ClassIsChildOf(Hit.GetActor()->GetClass(), ABreakableCube::StaticClass()))
			{
				TObjectPtr<ABreakableCube> HitCube = Cast<ABreakableCube>(Hit.GetActor());
				HitCube->AddInstancedPhysicsObject(Hit.Item);
				HitCube->PhysicsMoveToLocation = Start;
			}
		}
		LastHitResults = HitResults;
	}

}