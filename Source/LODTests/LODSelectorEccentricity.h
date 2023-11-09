// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "LODSelectorEccentricity.generated.h"

UCLASS()
class LODTESTS_API ALODSelectorEccentricity : public AActor
{
	GENERATED_BODY()
	
public:	
	TArray<UStaticMesh*> relevant_static_meshes;
	size_t num_ticks = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Components")
	FString modelname;

	// Sets default values for this actor's properties
	ALODSelectorEccentricity();

	// Called every frame
	virtual void Tick(float DeltaTime) override;


protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
};
