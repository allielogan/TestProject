// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Kismet/KismetSystemLibrary.h" 
#include "BreakableCube.h"
#include "BreakableAttachment.generated.h"

class ABreakableCube;

UCLASS( Blueprintable, meta=(BlueprintSpawnableComponent) )
class TESTPROJECT_API UBreakableAttachment : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UBreakableAttachment();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	UFUNCTION(BlueprintCallable, Category = Trace)
	void TraceForBreakableInRange();
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = Setup)
	TObjectPtr<ACharacter> AttachedCharacter = nullptr;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = Setup)
	TObjectPtr<AActor> AttachedActor = nullptr;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = Setup)
	FVector BreakableObjectMoveLoc;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = Setup)
	float AffectedRadius = 100.f;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = Trace)
	TArray<FHitResult> LastHitResults;
};
