// Fill out your copyright notice in the Description page of Project Settings.

#pragma once


/*
	LODSelector.h/cpp (ALODSelector) is just a very simple test
	to test out modifying static mesh LOD's.
*/

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"

#include "Engine/StaticMesh.h"


#include "LODSelector.generated.h"

UCLASS()
class LODTESTS_API ALODSelector : public AActor
{
	GENERATED_BODY()
	
public:

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Model")
	UStaticMesh *lod_object;

	size_t num_ticks = 0;

	// Sets default values for this actor's properties
	ALODSelector();

	// Called every frame
	virtual void Tick(float DeltaTime) override;

	void get_lod_object_data();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

};
