// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "TestProjectGameMode.generated.h"

UCLASS(minimalapi)
class ATestProjectGameMode : public AGameModeBase
{
	GENERATED_BODY()

public:
	ATestProjectGameMode();

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = Setup)
	TObjectPtr<APawn> PlayerPawn;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = Setup)
	int ScoreTotal = 0;

	void AddScore(int Points);

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = Setup)
	bool isGameStarted = false;
};



